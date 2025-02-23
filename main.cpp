#include "library/library.hpp"
#include "library/matrix.hpp"
#include "library/vectors.hpp"

#include <iostream>

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;

int main()
{
	Image image(ImageWidth, ImageHeight);

	image.write_file("output.png");
	return 0;
}
