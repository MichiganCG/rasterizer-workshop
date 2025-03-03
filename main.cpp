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

	Matrix4 m_projection = perspective_projection(90, aspect_ratio, 0.1, 100);
	Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

	// Load models
	Mesh model("cube.obj");
	Material material("material.mtl");

	// Set the object's transformation
	Vec4 object_position(0, 0, -4);
	Quaternion object_rotation({1, 1, 0}, 0.7);
	Matrix4 m_model;
	rotate(m_model, object_rotation);
	translate(m_model, object_position);

	Matrix4 m_total = m_projection * m_model;

	LightCollection lights;
	DirectionalLight l1({1, 0, 0}, {1, 1, 1});
	DirectionalLight l2({0, 1, 0}, {-1, 1, 1});
	DirectionalLight l3({0, 0, 1}, {0, -1, 1});
	lights.push_back(&l1);
	lights.push_back(&l2);
	lights.push_back(&l3);

	for (auto &face : model)
	{
		std::vector<Vertex> vertices(face.size);

		// Transform vertices from model space to clip space
		for (size_t i = 0; i < face.size; ++i)
		{
			vertices[i].point = m_total * face.get_vertex(i);
			vertices[i].normal = m_model * face.get_normal(i);
		}

		Vec4 normal = cross(vertices[1].point - vertices[0].point, vertices[2].point - vertices[0].point);
		if (normal.z < 0)
			continue;

		// Clip triangles to be bounded within [-w, w] on all axes.
		sutherland_hodgman_clip(vertices);

		if (vertices.size() == 0) // Skip triangles that are not on the screen
			continue;

		// Move triangles from clip space to NDC and then screen space.
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			Vertex &vertex = vertices[i];
			if (vertex.point.w != 0)
			{
				vertex.point /= vertex.point.w;
				vertex.point.w = 1;
			}

			vertex.screen = m_screen * vertex.point;
			// vertex.screen.w = std::clamp(vertex.screen.w, -1.0f, 1.0f);
		}

		// Split polygon into triangles using Fan Triangulation: https://en.wikipedia.org/wiki/Fan_triangulation
		for (size_t i = 0; i < vertices.size() - 1; ++i)
		{
			// Color color(1);
			// draw_barycentric(image, depth, color, vertices[0], vertices[i], vertices[i + 1]);
			draw_barycentric(image, depth, material, lights, vertices[0], vertices[i], vertices[i + 1]);
		}
	}

	image.write_file("output.png");

	return 0;
}
