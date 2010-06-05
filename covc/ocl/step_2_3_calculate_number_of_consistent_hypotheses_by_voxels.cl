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
calculate_number_of_consistent_hypotheses_by_voxels (__global uchar * hypotheses,
                                                     __global __const uint * dimensions,
                                                     uint number_of_images)
{
    uint4 voxel_pos = (uint4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);

    uint hypotheses_size = 1 + number_of_images;
    uint hypothesis_offset = voxel_pos.x*hypotheses_size +
                             voxel_pos.y*dimensions[0]*hypotheses_size +
                             voxel_pos.z*dimensions[0]*dimensions[1]*hypotheses_size;

    // if voxel is not visible
    uchar4 voxel_info = vload4(hypothesis_offset, hypotheses);
    if (voxel_info.x == 0)
        return;

    uchar consistent_hypotheses = 0;

    for (uint i = 0; i < number_of_images; ++i)
    {
        uchar4 color = vload4(hypothesis_offset + 1 + i, hypotheses);

        // if hypothesis is consistent
        if ((color.x + color.y + color.z + color.w) != 0)
            consistent_hypotheses++;
    }

    if (consistent_hypotheses == 0)
    {
        // make voxel invisible
        vstore4((uchar4)(0), hypothesis_offset, hypotheses);
    }
    else if (voxel_info.y != consistent_hypotheses)
    {
        // if number of consistent hypotheses at previos step
        // is not equal to number of consistent hypothises at this step
        voxel_info.y = consistent_hypotheses;
        vstore4(voxel_info, hypothesis_offset, hypotheses);
    }
}
