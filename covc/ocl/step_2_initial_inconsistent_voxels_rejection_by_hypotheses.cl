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
initial_inconsistent_voxels_rejection ( __global uchar * hypotheses,
                                        uint x, uint y, uint z,
                                        __local __read_only uint * dimensions,
                                        float threshold)
{
    // current hypothesis number
    uint pos = get_global_id(0);

    uint number_of_images = get_global_size(0);

    uint hypotheses_size = 2 + number_of_images*3;
    uint hypotheses_offset = 2 + z*hypotheses_size +
                             y*dimensions[2]*hypotheses_size +
                             x*dimensions[2]*dimensions[1]*hypotheses_size;

    // if voxel not visible
    if (hypotheses[hypotheses_offset + pos*3 - 2] == 0)
        return;

    uint4 hypothesis_color = (uint4) (hypotheses[hypotheses_offset + pos*3],
                                      hypotheses[hypotheses_offset + pos*3 + 1],
                                      hypotheses[hypotheses_offset + pos*3 + 2],
                                      0);

    // if hypothesis is not consist
    if (isequalui(hypothesis_color, (uint4)(0)))
        return;

    uint consistent = 0;
    for (uint i = 0; i < number_of_images; ++i)
    {
        uint current_offset = hypotheses_offset + i*3;

        // if it is the same hypothesis
        if (current_offset == (hypotheses_offset + pos*3))
            continue;

        uint4 color = (uint4) ( hypotheses[current_offset],
                                hypotheses[current_offset + 1],
                                hypotheses[current_offset + 2],
                                0);

        if (distance(normalize(convert_float4(color)), normalize(convert_float4(hypothesis_color))) < threshold)
        {
            consistent = 1;
            break;
        }
    }

    // hypothesis is not consistent
    if (!consistent)
    {
        hypotheses[hypotheses_offset + pos*3] = 0;
        hypotheses[hypotheses_offset + pos*3 + 1] = 0;
        hypotheses[hypotheses_offset + pos*3 + 2] = 0;
    }
}
