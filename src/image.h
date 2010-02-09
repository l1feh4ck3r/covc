#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>

#include "matrix/include/matrix.h"
using namespace math;

class Image
{
public:
    Image();
    Image(QString &filename, matrix<int> &_matrix_of_calibration, QRectF &_bounding_square);


public:
    bool is_valid () const {return valid;}
    const QImage & get_image () const {return image;}
    QRectF get_bounding_rectangle() const {return bounding_rectangle;}


private:
    QRectF      bounding_rectangle;
    QImage      image;
    matrix<int> matrix_of_calibration;
    bool        valid;
};

#endif // IMAGE_H
