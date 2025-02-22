#include "library/library.hpp"

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;

/**
 * Creates the 3D scene to be rendered.
 */
Scene make_scene()
{
	Scene scene;

	scene.insert_sphere({0.0f, 1.0f, 0.0f}, 1.0f);
	scene.insert_plane({0.0f, 1.0f, 0.0f}, 0.0f);

	return scene;
}

const Scene Scene = make_scene();

/**
 * Calculates the color value to output for a pixel.
 */
Color render_pixel(uint32_t x, uint32_t y)
{
	Color result;

	float u = (static_cast<float>(x) - ImageWidth / 2.0f) / ImageWidth;
	float v = (static_cast<float>(y) - ImageHeight / 2.0f) / ImageWidth;

	result = Color(u, v, 1.0f);
	return result;
}

int main()
{
	Image image(ImageWidth, ImageHeight);

	for (uint32_t y = 0; y < ImageHeight; ++y)
	{
		for (uint32_t x = 0; x < ImageWidth; ++x)
		{
			Color color = render_pixel(x, y);
			image.set_pixel(x, y, color);
		}
	}

	image.write_file("output.png");
	return 0;
}

