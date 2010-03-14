#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QImage>

#include "matrix/include/matrix.h"
using namespace math;

class ImageInfo
{
public:
    ImageInfo();
    ImageInfo(QString &filename, matrix<float> &_matrix_of_calibration, QRectF &_bounding_square);


public:
    bool is_valid () const {return valid;}
    const QImage & get_image () const {return image;}
    QRectF get_bounding_rectangle() const {return bounding_rectangle;}
    const matrix<float> & get_matrix_of_calibration() const {return matrix_of_calibration;}


private:
    QRectF      bounding_rectangle;
    QImage      image;
    matrix<float> matrix_of_calibration;
    bool        valid;
};

#endif // IMAGEINFO_H
