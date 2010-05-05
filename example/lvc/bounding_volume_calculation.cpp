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

#include "pictureinfo.h"

#include <vector>
using namespace std;

int calculate_bounding_volume (const vector<PictureInfo> & pictures,
                               matrix<float> & camera_calibration_matrix,
                               matrix<float> & bounding_volume)
{
    for (vector<PictureInfo>::const_iterator i = pictures.begin(); i < pictures.end(); ++i)
    {
        matrix<float> zero_zero_zero_one_vector(1, 4);
        zero_zero_zero_one_vector(0, 3) = 1;

        matrix<float> point_in_global_space(1, 3);
        matrix<float> camera_in_global_space((!camera_calibration_matrix)*(~zero_zero_zero_one_vector));

        //TODO: optimize it!

        for (size_t x = 0; x < 2; ++x)
        {
            for (size_t y = 0; y < 2; ++y)
            {
                matrix<float> corner_point(1, 3);

                // bounding rectangle: (x1,y1)(x2,y2)
                //                      0  1   2  3
                // getting points: (x1,y1), (x1,y2), (x2,y1), (x2,y2)
                //                 (0, 1),  (0, 3),  (2, 1),  (2, 3)

                corner_point(0, 0) = (*i).bounding_rectangle(0, x*2);
                corner_point(0, 1) = (*i).bounding_rectangle(0, 1+y*3);
                corner_point(0, 2) = 1.0f;

                matrix<float> point_in_picture_space = (!camera_calibration_matrix)*(~corner_point);
                point_in_global_space = (!((*i).matrix_of_calibration))*point_in_picture_space;

                //point_in_global_space = (*i)->
            }
        }
    }

    return 1;
}
