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

int main()
{
    stbi_flip_vertically_on_write(1);

    Image image(ImageWidth, ImageHeight);

    
    
    image.write_file("output.png");

    return 0;
}
