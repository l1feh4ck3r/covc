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

sampler_t imageSampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

float4 mul_mat_vec (float16 mat, float4 vec)
{
    float4 res;
    res.x = dot((float4)(mat.s0, mat.s1, mat.s2, mat.s3), vec);
    res.y = dot((float4)(mat.s4, mat.s5, mat.s6, mat.s7), vec);
    res.z = dot((float4)(mat.s8, mat.s9, mat.sA, mat.sB), vec);
    res.w = dot((float4)(mat.sC, mat.sD, mat.sE, mat.sF), vec);
    return res;
}

__kernel void
build_variety_of_hypotheses (__local __read_only float * bounding_box,
                            __read_only image3d_t images,
                            __global float16 * projection_matrices,
                            __global uchar * hypotheses,
                            __local __read_only uint * dimensions,
                            uint number_of_images)
{
    float4 pos3d = (float4) (bounding_box[0] + get_global_id(0)*(bounding_box[4]/dimensions[0]),
                             bounding_box[1] + get_global_id(1)*(bounding_box[5]/dimensions[1]),
                             bounding_box[2] + get_global_id(2)*(bounding_box[6]/dimensions[2]),
                             1);

    int width = get_image_width(images);
    int height = get_image_height(images);

    uint4 voxel_pos = (uint4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);

    uint hypotheses_size = 2+number_of_images*3;
    uint hypotheses_offset = 2 + voxel_pos.z*hypotheses_size +
                             voxel_pos.y*dimensions[2]*hypotheses_size +
                             voxel_pos.x*dimensions[2]*dimensions[1]*hypotheses_size;

    hypotheses[hypotheses_offset - 2] = 1;  // set voxel visible
    hypotheses[hypotheses_offset - 1] = UINT_MAX;  // set non zero number of consists hypotheses

    for (uint i = 0; i < number_of_images; ++i)
    {
        float4 pos_at_image_3d = mul_mat_vec(projection_matrices[i], pos3d);
        float4 pos_at_image = (float4) (pos_at_image_3d.x/pos_at_image_3d.z, pos_at_image_3d.y/pos_at_image_3d.z, i, 0);

        uint hypothesis_offset = hypotheses_offset + i*3;

        if (pos_at_image.x < 0 || pos_at_image.x > width ||
            pos_at_image.y < 0 || pos_at_image.y > height)
        {
            //if voxel not projected in image
            hypotheses[hypothesis_offset] = 0;     //r
            hypotheses[hypothesis_offset + 1] = 0; //g
            hypotheses[hypothesis_offset + 2] = 0; //b
        }
        else
        {
            //if voxel is projected in image

            uint4 color = read_imageui(images, imageSampler, pos_at_image);

            hypotheses[hypothesis_offset] = color.x;     //r
            hypotheses[hypothesis_offset + 1] = color.y; //g
            hypotheses[hypothesis_offset + 2] = color.z; //b
        }
    }
}
