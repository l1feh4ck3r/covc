#include "imageinfo.h"

ImageInfo::ImageInfo()
        :valid(false)
{
}

ImageInfo::ImageInfo(QString &filename, matrix<int> &_matrix_of_calibration, QRectF &_bounding_rectangle)
        :valid(false)
{
    if (image.load(filename))
    {
        matrix_of_calibration = _matrix_of_calibration;
        bounding_rectangle = _bounding_rectangle;
        valid = true;
    }
}
