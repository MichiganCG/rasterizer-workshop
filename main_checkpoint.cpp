#include "library/vectors.hpp"
#include "library/quaternion.hpp"
#include "library/matrix.hpp"
#include "library/mesh.hpp"
#include "library/light.hpp"
#include "library/render.hpp"
#include "library/library.hpp"

#include <string>
#include <iostream>

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;
const float AspectRatio = (float)ImageWidth / (float)ImageHeight;

int main()
{
    stbi_flip_vertically_on_write(1);

    Image image(ImageWidth, ImageHeight);
    DepthBuffer depth(ImageWidth, ImageHeight);

    Matrix4 m_projection = perspective_projection(70, AspectRatio, 1, 100);
    Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

    // Define our object
    Mesh mesh("model/cube.obj");

    Vec4 position{0, 0, -5};
    Quaternion rotation{{1, 1, 0}, Pi / 3};

    // Define the model matrix
    Matrix4 m_model = translate(position) * rotate(rotation);

    VertexBuffer vertices{mesh.vertex_size()};

    // Transform the model to world space
    for (size_t i = 0; i < mesh.vertex_size(); ++i)
    {
        vertices[i].world_coordinates = m_model * mesh.get_vertex(i);
        vertices[i].world_normals     = m_model * mesh.get_normal(i);

        Vec4 &clip = vertices[i].clip_coordinates;
        clip = m_projection * vertices[i].world_coordinates;

        // Scale by the depth
        float temp = clip.w;
        if (temp != 0) {
            temp = 1.0f / temp;
            clip *= temp;
        }

        vertices[i].screen_coordinates = m_screen * clip;

        clip.w = temp; // store the w value for later
    }

    // Draw each triangle
    for (size_t i = 0; i < mesh.size(); ++i)
    {
        Triplet triangle = mesh[i];
        const VertexBuffer::Vertex &v0 = vertices[triangle[0]], &v1 = vertices[triangle[1]], &v2 = vertices[triangle[2]];

        Color color(1);

        auto shader = [&](float a, float b, float c)
        {
            return color;
        };

        iterate_shader(image, depth, shader, v0.screen_coordinates, v1.screen_coordinates, v2.screen_coordinates);
    }

    image.write_file("output.png");

    return 0;
}
