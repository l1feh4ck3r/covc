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

//sampler_t imageSampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
sampler_t imageSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

float4 mul_mat_vec (float16 mat, float4 vec)
{
    float4 res;
    res.x = dot((float4)(mat.s0, mat.s1, mat.s2, mat.s3), vec);
    res.y = dot((float4)(mat.s4, mat.s5, mat.s6, mat.s7), vec);
    res.z = dot((float4)(mat.s8, mat.s9, mat.sA, mat.sB), vec);
    res.w = dot((float4)(mat.sC, mat.sD, mat.sE, mat.sF), vec);
    return res;
}

uint is_in_image(float4 pos, float4 box)
{
    if (isless(pos.x, box.x) || isgreater(pos.x, box.z) ||
        isless(pos.y, box.y) || isgreater(pos.y, box.w) )
        return 0;

    return 1;
}

__kernel void
build_variety_of_hypotheses (__global __const float * bounding_box,
                            __read_only image3d_t images,
                            __global float16 * projection_matrices,
                            __global uchar * hypotheses,
                            __global __const uint * dimensions,
                            uint number_of_images)
{
    uint4 voxel_pos = (uint4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);

    float4 voxel_pos_3d = (float4) ((float)bounding_box[0] + ((float)voxel_pos.x + 0.5f)*((float)bounding_box[3]/(float)dimensions[0]),
                                    (float)bounding_box[1] + ((float)voxel_pos.y + 0.5f)*((float)bounding_box[4]/(float)dimensions[1]),
                                    (float)bounding_box[2] + ((float)voxel_pos.z + 0.5f)*((float)bounding_box[5]/(float)dimensions[2]),
                                     1.0f);

    int width = get_image_width(images);
    int height = get_image_height(images);


    __const uint hypotheses_offset = voxel_pos.x*(1 + number_of_images) +
                                     voxel_pos.y*dimensions[0]*(1 + number_of_images) +
                                     voxel_pos.z*dimensions[0]*dimensions[1]*(1 + number_of_images);

    // set voxel visible and non zero number of consists hypotheses
    vstore4((uchar4)(1, UCHAR_MAX, 0, 0), hypotheses_offset, hypotheses);

    for (uint i = 0; i < number_of_images; ++i)
    {
        float4 pos_at_image_3d = mul_mat_vec(projection_matrices[i], voxel_pos_3d);
        float4 pos_at_image = (float4) (pos_at_image_3d.x/pos_at_image_3d.z, pos_at_image_3d.y/pos_at_image_3d.z, i, 0);

        uint hypothesis_offset = hypotheses_offset + 1 + i;

        if (is_in_image(pos_at_image, (float4)(0.0f, 0.0f, convert_float(width), convert_float(height))))
        {
            //if voxel not projected in image
            vstore4((uchar4)(0), hypothesis_offset, hypotheses);
        }
        else
        {
            //if voxel is projected in image

            // we have ARGB format
            uint4 color = read_imageui(images, imageSampler, pos_at_image);
            color.w = 0;

            if (color.x < 10 && color.y < 10 && color.z < 10)
                vstore4((uchar4)(0), hypothesis_offset, hypotheses);
            else
                vstore4((uchar4)(color.x, color.y, color.z, 0), hypothesis_offset, hypotheses);
        }
    }
}
