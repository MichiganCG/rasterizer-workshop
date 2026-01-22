#include "library/vectors.hpp"
#include "library/quaternion.hpp"
#include "library/matrix.hpp"
#include "library/mesh.hpp"
#include "library/light.hpp"
#include "library/library.hpp"

#include <string>
#include <iostream>

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;
const float AspectRatio = (float)ImageWidth / (float)ImageHeight;

int main()
{
    Image image(ImageWidth, ImageHeight);
    DepthBuffer depth(ImageWidth, ImageHeight);

    Matrix4 m_projection = perspective_projection(70, AspectRatio, 0.1, 100);
    Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

    // Define our object
    Mesh mesh("model/cube.obj");

    Vec4 position{0, 0, -5};
    Quaternion rotation{{1, 1, 0}, Pi / 3};

    // Define the model matrix
    Matrix4 m_model = translate(position) * rotate(rotation);

    std::vector<Vec4> world_vertices(mesh.vertex_size());
    std::vector<Vec4> world_normals(mesh.vertex_size());

    std::vector<Vec4> clip_vertices(mesh.vertex_size());

    // Transform the model to world space
    for (size_t i = 0; i < mesh.vertex_size(); ++i)
    {
        world_vertices[i] = m_model * mesh.get_vertex(i);
        world_normals[i] = m_model * mesh.get_normal(i);

        clip_vertices[i] = m_projection * world_vertices[i];
    }

    for (size_t i = 0; i < mesh.size(); ++i)
    {
        Triangle triangle = mesh[i];
        std::vector<Vertex> vertices(3);

        for (size_t t = 0; t < 3; ++t)
        {
            vertices[t].world = world_vertices[triangle[t]];
            vertices[t].clip = clip_vertices[triangle[t]];
            vertices[t].normal = world_normals[triangle[t]];
            vertices[t].texture = mesh.get_texture(triangle[t]);
        }

        // Clip triangles such that they are bounded within [-w, w] on all axes
        sutherland_hodgman(vertices);

        if (vertices.size() == 0)
            continue;

        // Transform from clip space to screen space
        for (size_t j = 0; j < vertices.size(); ++j)
        {
            Vertex &vertex = vertices[j];
            Vec4 &clip = vertex.clip;

            float temp = clip.w;
            if (temp != 0)
            {
                temp = 1.0f / temp;
                clip *= temp;
            }

            vertex.screen = m_screen * clip;

            clip.w = temp;
        }

        // Backface culling
        Vec4 ab = vertices[1].clip - vertices[0].clip, ac = vertices[2].clip - vertices[0].clip;

        float orientation = ab.x * ac.y - ac.x * ab.y;
        if (orientation < 0.0f) continue;

        Color color(1);

        // Reform triangles using fan triangulation
        for (size_t j = 1; j < vertices.size() - 1; ++j)
        {
            Vertex &v0 = vertices[0], &v1 = vertices[j], &v2 = vertices[j + 1];

            auto shader = [&](float a, float b, float c)
            {
                return color;
            };

            iterate_barycentric(image, depth, shader, v0.screen, v1.screen, v2.screen);
        }
    }

    image.write_file("output.png");

    return 0;
}
