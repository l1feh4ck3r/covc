#include "image.h"

Image::Image()
        :valid(false)
{
}

Image::Image(QString &filename, matrix<int> &_matrix_of_calibration, QRectF &_bounding_rectangle)
        :valid(false)
{
    if (image.load(filename))
    {
        matrix_of_calibration = _matrix_of_calibration;
        bounding_rectangle = _bounding_rectangle;
        valid = true;
    }
}
