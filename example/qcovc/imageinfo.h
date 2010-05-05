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

#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QImage>

#include "matrix.h"
using namespace math;

class ImageInfo
{
public:
    ImageInfo();
    ImageInfo(QString &filename);
    ImageInfo(QString &filename, matrix<float> &_matrix_of_calibration, QRectF &_bounding_square);


public:
    bool is_valid () const {return valid;}

    const QImage &          get_image () const {return image;}
    QRectF                  get_bounding_rectangle() const {return bounding_rectangle;}
    const matrix<float> &   get_matrix_of_calibration() const {return matrix_of_calibration;}
    const QString &         get_path_to_image () const {return path_to_image;}

    void set_image (const QImage &_image);
    void set_bounding_rectangle(const QRectF &_bounding_rectangle);
    void set_matrix_of_calibration(const matrix<float> &_matrix_of_calibration);
    void set_element_of_matrif_of_calibration(int row, int column, float value);


private:
    QRectF      bounding_rectangle;
    QImage      image;
    matrix<float> matrix_of_calibration;
    bool        valid;
    QString     path_to_image;
};

#endif // IMAGEINFO_H
