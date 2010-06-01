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


float4 mul_mat_vec (float16 mat, float4 vec)
{
    float4 res;
    res.x = dot((float4)(mat.s0, mat.s1, mat.s2, mat.s3), vec);
    res.y = dot((float4)(mat.s4, mat.s5, mat.s6, mat.s7), vec);
    res.z = dot((float4)(mat.s8, mat.s9, mat.sA, mat.sB), vec);
    res.w = dot((float4)(mat.sC, mat.sD, mat.sE, mat.sF), vec);
    return res;
}


uint isequalui(uint4 vec1, uint4 vec2)
{
    if (vec1.x != vec2.x || vec1.y != vec2.y ||
        vec1.z != vec2.z || vec1.w != vec2.w)
        return 0;
    else
        return 1;
}

//TODO: remove this hardcoded image sizes
#define width   512
#define height  512

__kernel void
inconsistent_voxels_rejection ( __global uchar * hypotheses,
                                uint x, uint y, uint z,
                                __local __read_only float * bounding_box,
                                __local __read_only uint * dimensions,
                                __global uchar * z_buffer,
                                __global float16 * projection_matrices,
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


    // project voxel to z buffer
    // calculate voxel position in 3d
    float4 pos3d = (float4) (bounding_box[0] + x*(bounding_box[4]/dimensions[0]),
                             bounding_box[1] + y*(bounding_box[5]/dimensions[1]),
                             bounding_box[2] + z*(bounding_box[6]/dimensions[2]),
                             1);

    // calculate voxel pos in voxel model
    uint4 voxel_pos = (uint4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
    float4 pos_at_image_3d = mul_mat_vec(projection_matrices[pos], pos3d);
    float4 pos_at_image = (float4) (pos_at_image_3d.x/pos_at_image_3d.z, pos_at_image_3d.y/pos_at_image_3d.z, pos, 0);

    uint z_buffer_offset = pos_at_image.z + pos_at_image.y*number_of_images + pos_at_image.x*height*number_of_images;
    // if hypothesis occupied
    if (z_buffer[z_buffer_offset] == 1)
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
    else
    {
        //set hypothesis occupied on z buffer
        z_buffer[z_buffer_offset] = 1;
    }
}
