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

uint is_in_image(float4 pos, float4 box)
{
    if (isless(pos.x, box.x) || isgreater(pos.x, box.y) ||
        isless(pos.y, box.z) || isgreater(pos.y, box.w) )
        return 0;

    return 1;
}


//TODO: remove this hardcoded image sizes
#define width   512
#define height  512

float4 position_at_image(float16 projection_matrix, float4 position_in_3d)
{
    float4 pos_at_image_3d = mul_mat_vec(projection_matrix, position_in_3d);
    float4 pos_at_image = (float4) (pos_at_image_3d.x/pos_at_image_3d.z, pos_at_image_3d.y/pos_at_image_3d.z, 0, 0);

    return pos_at_image;
}


__kernel void
inconsistent_voxel_rejection ( __global uchar * hypotheses,
                                uint x, uint y, uint z,
                                __global __const float * bounding_box,
                                __global __const uint * dimensions,
                                __global uint * z_buffer,
                                __global float16 * projection_matrices,
                                float threshold,
                                uint pos,
                                uint number_of_images)
{
    uint hypotheses_size = 1 + number_of_images;
    uint hypotheses_offset = x*hypotheses_size +
                             y*dimensions[0]*hypotheses_size +
                             z*dimensions[0]*dimensions[1]*hypotheses_size;

    // if voxel not visible
    uchar4 voxel_info = vload4(hypotheses_offset, hypotheses);
    if (voxel_info.x == 0)
        return;

    uchar4 hypothesis_color = vload4(hypotheses_offset + 1 + pos, hypotheses);

    // if hypothesis is not consist
    if ((hypothesis_color.x + hypothesis_color.y + hypothesis_color.z + hypothesis_color.w) == 0)
        return;

    // project voxel to z buffer
    // calculate voxel position in 3d
    float4 pos3d = (float4) (convert_float(bounding_box[0]) + convert_float(x)*(convert_float(bounding_box[4])/convert_float(dimensions[0])),
                             convert_float(bounding_box[1]) + convert_float(y)*(convert_float(bounding_box[5])/convert_float(dimensions[1])),
                             convert_float(bounding_box[2]) + convert_float(z)*(convert_float(bounding_box[6])/convert_float(dimensions[2])),
                             1);

    // calculate voxel pos in voxel model
    float4 pos_at_image = position_at_image(projection_matrices[pos], pos3d);
    pos_at_image.z = pos;

    if (is_in_image(pos_at_image, (float4)(0.0f, 0.0f, convert_float(width), convert_float(height))))
        return;

    uint z_buffer_offset = convert_uint((pos_at_image.x/512.0f)*32.0f) +
                           convert_uint((pos_at_image.y/512.0f)*32.0f)*32 +
                           pos*32*32;
//uint z_buffer_offset = convert_uint(pos_at_image.x) +
//                       convert_uint(pos_at_image.y)*width +
//                       pos*width*height;

    // if hypothesis occupied
    uint occupied = z_buffer[z_buffer_offset];
    if (occupied)
        return;

    uint consistent = 0;
    for (uint i = 0; i < number_of_images && consistent == 0; ++i)
    {
        float4 pos_at_second_image = position_at_image(projection_matrices[i], pos3d);
        pos_at_second_image.z = i;

        if (is_in_image(pos_at_second_image, (float4)(0.0f, 0.0f, convert_float(width), convert_float(height))))
            continue;

        uint z_buffer_offset_second = convert_uint((pos_at_second_image.x/512.0f)*32.0f) +
                               convert_uint((pos_at_second_image.y/512.0f)*32.0f)*32 +
                               i*32*32;

//        uint z_buffer_offset_second = convert_uint(pos_at_second_image.x) +
//                                      convert_uint(pos_at_second_image.y)*width +
//                                      i*width*height;

        // if hypothesis occupied
        uint occupied_second = z_buffer[z_buffer_offset_second];
        if (!occupied_second)
        {
            uint current_offset = hypotheses_offset + 1 + i;

            // if it is the same hypothesis
            if (i != pos)
            {
                uchar4 color = vload4 (current_offset, hypotheses);


                if (distance(normalize(convert_float4(color)), normalize(convert_float4(hypothesis_color))) < threshold)
                    consistent = 1;
            }
        }
    }

    // hypothesis is not consistent
    if (!consistent)
        vstore4((uchar4)(0), hypotheses_offset + 1 + pos, hypotheses);
    else
        //set hypothesis occupied on z buffer
        z_buffer[z_buffer_offset]++;
}
