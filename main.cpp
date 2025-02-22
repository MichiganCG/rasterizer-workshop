#include "library/library.hpp"
#include "library/matrix.hpp"

#include <iostream>

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;

int main()
{
	Image image(ImageWidth, ImageHeight);

	// Loop through all objects
	Matrix4 m;
	m[0][0] = 1.0f;
	std::cout << m[0][0] << std::endl;

	image.write_file("output.png");
	return 0;
}
