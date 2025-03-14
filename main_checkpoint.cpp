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
const float aspect_ratio = (float)ImageWidth / (float)ImageHeight;

struct Object
{
    Vec4 position;
    Quaternion rotation;
    Vec3 scale;
    Mesh &mesh;
};

int main()
{
    Image image(ImageWidth, ImageHeight);
    DepthBuffer depth(image);

    Matrix4 m_projection = perspective_projection(70, aspect_ratio, 0.1, 100);
    Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

    // Load models
    Mesh cube("model/cube.obj");

    // Set lights
    LightCollection lights;
    DirectionalLight l1(lights, {1, 1, 1}, {0, -1, 0, 0});

    // Set objects
    std::vector<Object> objects;
    objects.push_back({{0, 0, -5}, {{1, 1, 0}, Pi / 3}, {1}, cube});

    for (Object &object : objects)
    {
        Mesh &mesh = object.mesh;
        // Define the model matrix
        Matrix4 m_model = translate(object.position) * rotate(object.rotation) * scale(object.scale);

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

            // Backface culling
            Vec4 view_vector = -normalize(vertices[0].world);
            Vec4 face_normal = normalize(cross(vertices[1].world - vertices[0].world, vertices[2].world - vertices[0].world));
            if (dot(view_vector, face_normal) < 0)
                continue;

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

            float angle = std::pow((dot(face_normal, l1.get_direction(vertices[0].world)) + 1) * 0.5, 2);
            Color color(angle);

            // Reform triangles using fan triangulation
            for (size_t j = 1; j < vertices.size() - 1; ++j)
            {
                Vertex &v0 = vertices[0], &v1 = vertices[j], &v2 = vertices[j + 1];

                float z0 = 1.0f / v0.screen.z, z1 = 1.0f / v1.screen.z, z2 = 1.0f / v2.screen.z;

                auto draw = [&](uint32_t u, uint32_t v, float a, float b, float c)
                {
                    float z = 1 / (a * z0 + b * z1 + c * z2);
                    // Check if this pixel is closer to the screen
                    if (z <= depth.at(u, v))
                    {
                        depth.at(u, v) = z;

                        image.set_pixel(u, v, color);
                    }
                };

                iterate_barycentric(image.get_width(), image.get_height(), draw, v0.screen, v1.screen, v2.screen);
            }
        }
    }

    image.write_file("output.png");
    return 0;
}
