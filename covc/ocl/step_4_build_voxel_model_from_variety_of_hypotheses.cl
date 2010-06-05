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

#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable


__kernel void
build_voxel_model ( __global uchar * hypotheses,
                    __global uchar * voxel_model,
                    __global __const uint * dimensions,
                    uint number_of_images)
{
    uint4 pos = (uint4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);

    __const uint hypothesis_size = (1 + number_of_images);
    __const uint hypothesis_offset = pos.z*hypothesis_size + pos.y*dimensions[2]*hypothesis_size +
                                     pos.x*dimensions[2]*dimensions[1]*hypothesis_size;

    uint4 result_color = (uint4)(0);
    uint  result_number_of_hypotheses = 0;

    uchar4 voxel_info = vload4(hypothesis_offset, hypotheses);

    // if voxel is visible
    if (voxel_info.x != 0)
    {
        for (uint i = 0; i < number_of_images; ++i)
        {
            uchar4 color = vload4(hypothesis_offset + 1 + i, hypotheses);

            // if hypothesis is consistent
            if ((color.x + color.y + color.z + color.w) != 0)
            {
                result_color.x += color.x;
                result_color.y += color.y;
                result_color.z += color.z;
                result_color.w += color.w;
                result_number_of_hypotheses++;
            }
        }

        result_color /= result_number_of_hypotheses;

        vstore4((uchar4)(0, result_color.x, result_color.y, result_color.z),
                pos.x + pos.y*dimensions[0] + pos.z*dimensions[0]*dimensions[1],
                voxel_model);
    }
    else
        vstore4((uchar4)(0, 0, 0, 0), pos.x + pos.y*dimensions[0] + pos.z*dimensions[0]*dimensions[1], voxel_model);

 }
