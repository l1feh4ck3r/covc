/*
 * Copyright (c) 2010 Alexey 'l1feh4ck3r' Antonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "imageinfo.h"

ImageInfo::ImageInfo()
        :valid(false)
{
}


ImageInfo::ImageInfo(QString &filename)
        :valid(false),
        matrix_of_calibration(4, 4)
{
    QImage temp_image;
    if (!temp_image.load(filename))
        return;

    path_to_image = filename;

    //TODO: magic numbers
    image = temp_image.scaled(512, 512,
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation).convertToFormat(QImage::Format_RGB32);
    valid = true;
}


ImageInfo::ImageInfo(QString &filename, matrix<float> &_matrix_of_calibration, QRectF &_bounding_rectangle)
        :valid(false),
        matrix_of_calibration(4, 4)
{
    QImage temp_image;
    if (!temp_image.load(filename))
        return;

    path_to_image = filename;

    //TODO: magic numbers
    image = temp_image.scaled(512, 512,
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation).convertToFormat(QImage::Format_RGB32);

    matrix_of_calibration = _matrix_of_calibration;
    bounding_rectangle = _bounding_rectangle;
    valid = true;
}


void ImageInfo::set_element_of_matrif_of_calibration(int row, int column, float value)
{
    matrix_of_calibration(row, column) = value;
}

void ImageInfo::set_image (const QImage &_image)
{
    image=_image;
}


void ImageInfo::set_bounding_rectangle(const QRectF &_bounding_rectangle)
{
    bounding_rectangle = _bounding_rectangle;
}


void ImageInfo::set_matrix_of_calibration(const matrix<float> &_matrix_of_calibration)
{
    matrix_of_calibration = _matrix_of_calibration;
}
