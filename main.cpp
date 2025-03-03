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

const std::vector<Plane> clipping_planes = {
	{{-1, 0, 0}, {1, 0, 0, 0}},
	{{1, 0, 0}, {-1, 0, 0, 0}},
	{{0, -1, 0}, {0, 1, 0, 0}},
	{{0, 1, 0}, {0, -1, 0, 0}},
	{{0, 0, -1}, {0, 0, 1, 0}},
	{{0, 0, 1}, {0, 0, -1, 0}},
};

int main()
{
	Image image(ImageWidth, ImageHeight);
	DepthBuffer depth(image);

	// Load models
	Mesh model("uv_sphere.obj");
	Material material("material.mtl");

	std::vector<DirectionalLight> lights = {
		{normalize({1, 1, 1}), {1, 0, 0}},
		{normalize({-1, 1, 1}), {0, 1, 0}},
		{normalize({-1, -1, 1}), {0, 0, 1}},
	};

	// Set the object's transformation
	Vec3 object_position(0, 0, -5);
	Quaternion object_rotation({0, 1, 2}, 1.5);
	Matrix4 m_model;
	rotate(m_model, object_rotation);
	translate(m_model, object_position);

	// Set the projection matrix
	Matrix4 m_projection = perspective_projection(90, aspect_ratio, 0.1, 1000);

	// Set the viewspace matrix
	Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

	Matrix4 m_total = m_projection * m_model;

	for (auto &face : model)
	{
		std::vector<Vec3> vertices(face.size);
		std::vector<Vec3> normals(face.size);
		// Transform vertices from model space to clip space
		bool display = false;
		for (size_t i = 0; i < face.size; ++i)
		{
			vertices[i] = m_total * face.get_vertex(i);
			normals[i] = m_model * face.get_normal(i);
			if (normals[i].z >= 0)
				display = true;
		}

		if (!display)
			continue;

		// Clip triangles to be bounded within [-1, 1] on all axes.
		vertices = sutherland_hodgman(vertices, clipping_planes);

		if (vertices.size() == 0) // Skip triangles that are not on the screen
			continue;

		std::vector<Vec2> points;
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			Vec2 point = m_screen * vertices[i];
			points.push_back(point);
		}

		// Split polygon into triangles using Fan Triangulation: https://en.wikipedia.org/wiki/Fan_triangulation
		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			Triangle triangle(i, points, vertices, normals);
			draw_barycentric(image, depth, material, lights, triangle);
		}
	}

	image.write_file("output.png");

	return 0;
}
