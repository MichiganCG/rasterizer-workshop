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
	Material &material;
};

int main()
{
	Image image(ImageWidth, ImageHeight);
	DepthBuffer depth(image);

	Matrix4 m_projection = perspective_projection(70, aspect_ratio, 0.1, 100);
	Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

	// Load models and lights
	Mesh cube("model/cube.obj");
	Mesh sphere("model/uv_sphere.obj");
	Mesh plane("model/plane.obj");

	Material material("material/material.mtl");
	Material tile("material/tiles074.mtl");

	LightCollection lights;
	DirectionalLight l1({0.2, 0.5, 0.79}, {-1, -1, -1, 0});
	lights.push_back(&l1);
	PointLight l2({1, 0, 0}, 2, {0, 0.5, -4.5});
	lights.push_back(&l2);
	SpotLight l3({0.6, 0.8, 0.5}, 0.3, 1, {1, -0.5, -1, 0}, {-5, 3, 0});
	lights.push_back(&l3);

	// Define our objects
	std::vector<Object> objects;
	objects.push_back({{-1.2, 0, -5}, {}, {1}, sphere, material});
	objects.push_back({{1.2, 0, -5}, {}, {0.8}, cube, tile});
	objects.push_back({{0, -1, -5}, {}, {12}, plane, material});

	for (Object &object : objects)
	{
		Matrix4 m_model = translate(object.position) * rotate(object.rotation) * scale(object.scale);

		for (Mesh::Face &face : object.mesh)
		{
			std::vector<VertexData> vertices(face.size());

			// Transform vertices from model space to clip space
			for (size_t i = 0; i < face.size(); ++i)
			{
				vertices[i].world = m_model * face.get_vertex(i);
				vertices[i].clip = m_projection * vertices[i].world;

				vertices[i].normal = m_model * face.get_normal(i);

				vertices[i].texture_coordinate = face.get_texture(i);
			}

			// Clip triangles to be bounded within [-w, w] on all axes
			sutherland_hodgman(vertices);

			if (vertices.size() == 0) // Skip triangles that are not on the screen
				continue;

			// Move triangles from clip space to screen space
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				VertexData &vertex = vertices[i];
				if (vertex.clip.w != 0)
				{
					vertex.clip /= vertex.clip.w;
					vertex.clip.w = 1;
				}

				vertex.screen_coordinate = m_screen * vertex.clip;
			}

			// Split polygon into triangles using fan triangulation: https://en.wikipedia.org/wiki/Fan_triangulation
			for (size_t i = 0; i < vertices.size() - 1; ++i)
			{
				draw_barycentric(image, depth, object.material, lights, vertices[0], vertices[i], vertices[i + 1]);
			}
		}
	}
	image.write_file("output.png");

	return 0;
}
