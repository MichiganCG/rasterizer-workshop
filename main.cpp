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

int main()
{
	Image image(ImageWidth, ImageHeight);
	DepthBuffer depth(image);

	Matrix4 m_projection = perspective_projection(70, aspect_ratio, 0.1, 100);
	// Matrix4 m_projection = orthographic_projection(3 * aspect_ratio, 3, 0.1, 100);
	Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

	// Load models and lights
	Mesh mesh("suzanne.obj");
	Material material("material.mtl");

	LightCollection lights;
	DirectionalLight l1({1, 0.5, 0.5}, {-1, -1, -1});
	lights.push_back(&l1);

	// Set the object's transformation
	Vec4 object_position(0, 0, -5);
	Quaternion object_rotation({1, 1, -1}, Pi / 4);

	Matrix4 m_model = translate(object_position) * rotate(object_rotation) * scale({1, 1, 1});
	Matrix4 m_total = m_projection * m_model;

	for (auto &face : mesh)
	{
		std::vector<VertexData> vertices(face.size);

		// Transform vertices from model space to clip space
		for (size_t i = 0; i < face.size; ++i)
		{
			vertices[i].position = m_total * face.get_vertex(i);
			vertices[i].normal = m_model * face.get_normal(i);
		}

		// Clip triangles to be bounded within [-w, w] on all axes
		sutherland_hodgman(vertices);

		if (vertices.size() == 0) // Skip triangles that are not on the screen
			continue;

		// Move triangles from clip space to screen space
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			VertexData &vertex = vertices[i];
			if (vertex.position.w != 0)
			{
				vertex.position /= vertex.position.w;
				vertex.position.w = 1;
			}

			vertex.screen_coordinate = m_screen * vertex.position;
		}

		// Split polygon into triangles using fan triangulation: https://en.wikipedia.org/wiki/Fan_triangulation
		for (size_t i = 0; i < vertices.size() - 1; ++i)
		{
			draw_barycentric(image, depth, material, lights, vertices[0], vertices[i], vertices[i + 1]);
		}
	}

	image.write_file("output.png");

	return 0;
}
