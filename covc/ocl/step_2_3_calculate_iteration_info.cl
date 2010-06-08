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

__kernel void
calculate_iteration_info (__global uchar * hypotheses,
                          __global __const uint * dimensions,
                          uint number_of_images,
                          __global __write_only uint * number_of_consistent_hypotheses)
{
    uint hypotheses_result = 0;
    uint voxels_result = 0;

    uint hypotheses_size = 1 + number_of_images;

    for (uint x = 0; x < dimensions[0]; ++x)
    {
        for (uint y = 0; y < dimensions[1]; ++y)
        {
            for (uint z = 0; z < dimensions[2]; ++z)
            {
                uint hypothesis_offset = x*hypotheses_size +
                                         y*dimensions[0]*hypotheses_size +
                                         z*dimensions[0]*dimensions[1]*hypotheses_size;

                // if voxel is visible
                uchar4 voxel_info = vload4(hypothesis_offset, hypotheses);

                if (voxel_info.x != 0)
                {
                    hypotheses_result += voxel_info.y;
                    voxels_result++;
                }
            }
        }
    }

    number_of_consistent_hypotheses[0] = hypotheses_result;
    number_of_consistent_hypotheses[1] = voxels_result;
}
