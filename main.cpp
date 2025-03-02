#include "library/vectors.hpp"
#include "library/quaternion.hpp"
#include "library/matrix.hpp"
#include "library/mesh.hpp"
#include "library/library.hpp"

#include <string>
#include <iostream>

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;

int main()
{
	const float aspect_ratio = (float)ImageWidth / (float)ImageHeight;

	std::vector<Plane> clipping_planes = {
		{{-1, 0, 0}, {1, 0, 0, 0}},
		{{1, 0, 0}, {-1, 0, 0, 0}},
		{{0, -1, 0}, {0, 1, 0, 0}},
		{{0, 1, 0}, {0, -1, 0, 0}},
		{{0, 0, 0}, {0, 0, 1, 0}},
		{{0, 0, 1}, {0, 0, -1, 0}},
	};

	Image image(ImageWidth, ImageHeight);
	DepthBuffer depth(ImageWidth, ImageHeight);

	Mesh model("model.obj");

	Vec3 obj(0, 0, -5);
	Quaternion rot;
	rot.rotate({0, 1, 0}, 0.5);
	Matrix4 m_model;
	translate(rotate(m_model, rot), obj);

	Matrix4 m_camera;
	m_camera.identity();

	Matrix4 m_projection = perspective_projection(90, aspect_ratio, 0.1, 1000);
	Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

	for (auto tri = model.begin(); tri != model.end(); ++tri)
	{
		std::vector<Vec3> vertices(3);
		// Transform vertices from model space to clip space
		for (size_t i = 0; i < 3; ++i)
		{
			vertices[i] = m_projection * m_camera * m_model * (*tri)[i];
		}

		// Clip triangles to be bounded within [-1, 1] on the x- and y-axes,
		// and between [0, 1] on the z-axis.
		vertices = sutherland_hodgman(vertices, clipping_planes);
		if (vertices.size() == 0)
			continue;

		std::vector<Vec2> points;
		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			Vec2 point = m_screen * *it;
			points.push_back(point);
		}

		for (size_t j = 0; j < points.size() - 1; ++j)
		{
			Vec2 &t1 = points[0];
			Vec2 &t2 = points[j];
			Vec2 &t3 = points[j + 1];
			draw_barycentric(image, depth, t1, t2, t3);
		}
	}

	image.write_file("output.png");

	return 0;
}
