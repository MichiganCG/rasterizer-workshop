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
	Image image(ImageWidth, ImageHeight);
	const float aspect = (float)ImageWidth / (float)ImageHeight;

	Mesh model("model.obj");

	Vec3 obj(0, 0, -5);
	Quaternion rot;
	rot.rotate({0, 1, 0}, 0.5);
	Matrix4 m_model;
	translate(rotate(m_model, rot), obj);

	Matrix4 m_camera;
	m_camera.identity();

	Matrix4 m_projection = perspective_projection(90, aspect, 0.1, 1000);
	Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

	std::vector<Plane> clipping_planes = {
		{{-1, 0, 0}, {1, 0, 0, 0}},
		{{1, 0, 0}, {-1, 0, 0, 0}},
		{{0, -1, 0}, {0, 1, 0, 0}},
		{{0, 1, 0}, {0, -1, 0, 0}},
		{{0, 0, 0}, {0, 0, 1, 0}},
		{{0, 0, 1}, {0, 0, -1, 0}},
	};

	for (auto tri = model.begin(); tri != model.end(); ++tri)
	{
		std::vector<Vec3> vertices(3);
		for (size_t i = 0; i < 3; ++i) // Transform vertices into clip space
		{
			vertices[i] = m_projection * m_camera * m_model * (*tri)[i];
		}

		// Clip triangles to be bounded within [-1, 1] on the x- and y-axes, and between [0, 1] on the z-axis.
		vertices = sutherland_hodgman(vertices, clipping_planes);

		if (vertices.size() == 0) continue;
		std::vector<Vec2> points;
		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			Vec2 point = m_screen * *it;
			points.push_back(point);
		}

        Vec2 *start = &points[points.size() - 1];
        for (size_t j = 0; j < points.size(); ++j) {
            Vec2 *end = &points[j];
            DDA(image, *start, *end);
			start = end;
		}
	}

	// image.set_pixel(0, 0, {1, 1, 1});
	image.write_file("output.png");

	return 0;
}
