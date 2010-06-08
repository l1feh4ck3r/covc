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
initial_inconsistent_voxels_rejection ( __global uchar * hypotheses,
                                        uint x, uint y, uint z,
                                        __global __const uint * dimensions,
                                        float threshold)
{
    // current hypothesis number
    uint pos = get_global_id(0);

    uint number_of_images = get_global_size(0);

    __const uint hypotheses_offset = x*(1 + number_of_images) +
                                     y*dimensions[0]*(1 + number_of_images) +
                                     z*dimensions[0]*dimensions[1]*(1 + number_of_images);

    // if voxel not visible
    uchar4 voxel_info = vload4(hypotheses_offset, hypotheses);
    if (voxel_info.x == 0)
        return;

    uchar4 hypothesis_color = vload4(hypotheses_offset + 1 + pos, hypotheses);

    // if hypothesis is not consist
    if ((hypothesis_color.x + hypothesis_color.y + hypothesis_color.z + hypothesis_color.w) == 0)
        return;

    uint consistent = 0;
    for (uint i = 0; i < number_of_images && consistent == 0; ++i)
    {
        uint current_offset = hypotheses_offset + 1 + i;

        // if it is not the same hypothesis
        if (i != pos)
        {
            uchar4 color = vload4(current_offset, hypotheses);

            if(isless( distance( normalize(convert_float4(color)), normalize(convert_float4(hypothesis_color))), threshold))
                consistent = 1;
        }
    }

    // hypothesis is not consistent
    if (consistent == 0)
    {
        vstore4((uchar4)(0), hypotheses_offset + 1 + pos, hypotheses);
    }
}
