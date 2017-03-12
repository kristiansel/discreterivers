#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../dep/stb/stb_image.h"

namespace IMG {

Ptr::OwningPtr<Image> loadImage(const char * filename)
{
    int x, y, n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
    return Ptr::OwningPtr<Image>(new Image{data, x, y, n});
}

const char * getError()
{
    return stbi_failure_reason();
}

Image::~Image()
{
    stbi_image_free(pixels);
}

} // namespace IMG
