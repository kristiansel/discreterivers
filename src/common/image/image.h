#ifndef IMAGE_H
#define IMAGE_H

#include "../pointer.h"

namespace IMG {

struct Image {
    unsigned char * pixels;
    int width;
    int height;
    int n_components;

    ~Image();
};

Ptr::OwningPtr<Image> loadImage(const char * filename);

const char * getError();

} // IMG

#endif // IMAGE_H
