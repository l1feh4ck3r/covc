#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>

#include "matrix/include/matrix.h"
using namespace math;

class Image
{
public:
    Image();
    Image(QString &filename, matrix<int> &_matrix_of_calibration, QPoint &_bounding_square);


public:
    bool isValid () const {return valid;}
    const QImage & getImage () const {return image;}


private:
    QPoint bounding_sqare;
    matrix<int>  matrix_of_calibration;
    QImage  image;
    bool valid;
};

#endif // IMAGE_H
