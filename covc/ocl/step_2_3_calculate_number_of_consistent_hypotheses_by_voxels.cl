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


uint isequalui(uint4 vec1, uint4 vec2)
{
    if (vec1.x != vec2.x || vec1.y != vec2.y ||
        vec1.z != vec2.z || vec1.w != vec2.w)
        return 0;
    else
        return 1;
}


__kernel void
calculate_number_of_consistent_hypotheses_by_voxels (__global uchar * hypotheses,
                                                     __global __const uint * dimensions,
                                                     uint number_of_images)
{
    uint4 voxel_pos = (uint4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);

    uint hypotheses_size = 2+number_of_images*3;

    uint hypothesis_offset = voxel_pos.z*hypotheses_size +
                             voxel_pos.y*dimensions[2]*hypotheses_size +
                             voxel_pos.x*dimensions[2]*dimensions[1]*hypotheses_size;

    // if voxel is not visible
    if (hypotheses[hypothesis_offset] == 0)
        return;

    uchar consistent_hypotheses = 0;

    for (uint i = 0; i < number_of_images; ++i)
    {
        uint4 color = (uint4)  (hypotheses[hypothesis_offset + 2 + i*3],
                                hypotheses[hypothesis_offset + 2 + i*3 + 1],
                                hypotheses[hypothesis_offset + 2 + i*3 + 2],
                                0);

        // if hypothesis is consistent
        if (isequalui(color, (uint4)(0)))
            consistent_hypotheses++;
    }

    // if number of consistent hypotheses at previos step
    // is not equal to number of consistent hypothises at this step
    if (hypotheses[hypothesis_offset + 1] != consistent_hypotheses)
        hypotheses[hypothesis_offset + 1] = consistent_hypotheses;
}
