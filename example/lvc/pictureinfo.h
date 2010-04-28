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

#ifndef PICTUREINFO_H
#define PICTUREINFO_H

#include <iostream>
#include <vector>

#include "matrix.h"
using namespace math;

struct Img
{
    union
    {
        unsigned char p[4];
        struct
        {
            unsigned char b, g, r, a;
        };
    };
};

class PictureInfo
{
public:
    PictureInfo();
    ~PictureInfo();


public:
    int init(const vector<float> &_bounding_rectangle,
             const matrix<float> &_matrix_of_calibration,
             const std::string & file_name);
    int load_jpg(const std::string & file_name);


private:
    vector<float> bounding_rectangle;
    matrix<float> matrix_of_calibration;
    bool        valid;

    // image info
    Img * bmap;
    int height;
    int width;
    int depth;
};

#endif  //  PICTUREINFO_H
