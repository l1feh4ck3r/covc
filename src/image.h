#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>

#include "matrix/include/matrix.h"
using namespace math;

class Image
{
public:
    Image();

private:
    Matrix  bounding_sqare;
    QImage  image;
};

#endif // IMAGE_H
