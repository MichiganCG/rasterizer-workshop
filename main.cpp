#include "library/vectors.hpp"
#include "library/quaternion.hpp"
#include "library/matrix.hpp"
#include "library/mesh.hpp"
#include "library/light.hpp"
#include "library/render.hpp"
#include "library/library.hpp"

#include <string>
#include <iostream>

//TODO...
Matrix4 projection(float fov, float aspect_ratio, float near, float far)
{
    Matrix4 matrix;



    return matrix;
}

//TODO...
Matrix4 screen(uint32_t width, uint32_t height)
{
    Matrix4 matrix;



    return matrix;
}

int main(int argc, char *argv[])
{
    stbi_flip_vertically_on_write(1);

    if (argc < 2)
    {
        std::cerr << "Error: Must provide a scene config" << "\n";
        return 1;
    }
    std::string config = argv[1];

    SceneManager manager;
    Scene scene(config, manager);

    Image image(scene.get_width(), scene.get_height());
    DepthBuffer depth(scene.get_width(), scene.get_height());

    Matrix4 m_projection = projection(scene.get_fov(), scene.get_aspect_ratio(), 1, 100);
    Matrix4 m_screen = screen(scene.get_width(), scene.get_height());

    for (const auto &object : scene.get_objects())
    {
        const Mesh &mesh = object->mesh;

        //TODO... Define the model matrix
        Matrix4 m_model = Matrix4::Identity;

        // Transform all vertices in the mesh to world space and then to clip space
        VertexBuffer vertices{mesh.vertex_size()};
        for (size_t i = 0; i < mesh.vertex_size(); ++i)
        {
            vertices[i].world_coordinates   = m_model * mesh.get_vertex(i);
            vertices[i].world_normals       = m_model * mesh.get_normal(i);
            vertices[i].clip_coordinates    = m_projection * vertices[i].world_coordinates;
            vertices[i].texture_coordinates = mesh.get_texture(i);
        }

        // Loop through all triangles in the mesh
        std::vector<Triplet> triangles;
        for (size_t i = 0; i < mesh.size(); ++i)
        {
            Triplet triangle = mesh[i];
            std::vector<uint32_t> indices{triangle.indices, triangle.indices + 3};

            // Clip triangles such that they are bounded within [-w, w] on all axes
            vertices.sutherland_hodgman_clip(indices);

            // Reform triangles using fan triangulation
            for (size_t j = 2; j < indices.size(); ++j)
                triangles.emplace_back(indices[0], indices[j - 1], indices[j]);
        }

        // Transform from clip space to screen space
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            Vec4 &clip = vertices[i].clip_coordinates;

            // Scale by the depth
            float temp = clip.w;
            if (temp != 0) {
                temp = 1.0f / temp;
                clip *= temp;
            }

            vertices[i].screen_coordinates = m_screen * clip;

            clip.w = temp; // store the w value for later
        }

        std::vector<Triplet> drawn_triangles;
        for (size_t i = 0; i < triangles.size(); ++i)
        {
            Triplet triangle = triangles[i];

            // Backface culling
            Vec4 ab = vertices[triangle[1]].clip_coordinates - vertices[triangle[0]].clip_coordinates;
            Vec4 ac = vertices[triangle[2]].clip_coordinates - vertices[triangle[0]].clip_coordinates;

            // Ignore triangles that are ordered incorrectly
            float orientation = ab.x * ac.y - ac.x * ab.y;
            if (orientation > 0.0f)
                drawn_triangles.emplace_back(triangle);
        }
        
        // Calculate the depth of each triangle
        for (auto &triangle : drawn_triangles)
            iterate_depth(depth, vertices[triangle[0]].screen_coordinates, vertices[triangle[1]].screen_coordinates, vertices[triangle[2]].screen_coordinates);

        // Draw each triangle
        for (auto &triangle : drawn_triangles)
        {
            const VertexBuffer::Vertex &v0 = vertices[triangle[0]], &v1 = vertices[triangle[1]], &v2 = vertices[triangle[2]];

            Color color(1);

            auto shader = [&](float a, float b, float c)
            {
                //TODO...



                return color;
            };

            iterate_shader(image, depth, shader, v0.screen_coordinates, v1.screen_coordinates, v2.screen_coordinates);
        }
    }
    
    image.write_file("output.png");

    return 0;
}
