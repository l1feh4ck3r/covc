#include "image.h"

Image::Image()
        :valid(false)
{
}

Image::Image(QString &filename, matrix<int> &_matrix_of_calibration, QPoint &_bounding_square)
        :valid(false)
{
    if (image.load(filename))
    {
        matrix_of_calibration = _matrix_of_calibration;
        bounding_sqare = _bounding_square;
        valid = true;
    }
}
