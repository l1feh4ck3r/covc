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

float4 position_at_image(float16 projection_matrix, float4 position_in_3d)
{
    float4 pos_at_image_3d = mul_mat_vec(projection_matrix, position_in_3d);
    float4 pos_at_image = (float4) (pos_at_image_3d.x/pos_at_image_3d.z, pos_at_image_3d.y/pos_at_image_3d.z, 0, 0);

    return pos_at_image;
}

// intersect ray with a box
// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

int intersectBox(float4 r_o, float4 r_d, float4 boxmin, float4 boxmax, float *tnear, float *tfar)
{
    // compute intersection of ray with all six bbox planes
    float4 invR = (float4)(1.0f,1.0f,1.0f,1.0f) / r_d;
    float4 tbot = invR * (boxmin - r_o);
    float4 ttop = invR * (boxmax - r_o);

    // re-order intersections to find smallest and largest on each axis
    float4 tmin = min(ttop, tbot);
    float4 tmax = max(ttop, tbot);

    // find the largest tmin and the smallest tmax
    float largest_tmin = max(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float smallest_tmax = min(min(tmax.x, tmax.y), min(tmax.x, tmax.z));

    *tnear = largest_tmin;
    *tfar = smallest_tmax;

    return smallest_tmax > largest_tmin;
}

// base on code from volumeRender.cl from NVIDIA GPU Computing SDK
int4 hit_voxel (float8 bounding_box,
                uint x, uint y, uint width, uint height,
                float4 eyeRay_o,
                float16 projection_matrix,
                uint4 dimensions,
                float step,
                int * find_voxel)
{
    // map to [-1, 1] coordinates
    float u = (x / (float) width)*2.0f-1.0f;
    float v = (y / (float) height)*2.0f-1.0f;

    float4 boxMin = (float4)(bounding_box.s0, bounding_box.s1, bounding_box.s2, 1.0f);
    float4 boxMax = (float4)(bounding_box.s0 + bounding_box.s3,
                             bounding_box.s1 + bounding_box.s4,
                             bounding_box.s2 + bounding_box.s5,
                             1.0f);

    // calculate eye ray in world space
    float4 eyeRay_d;

    float4 temp = (float4)(u, v, 1.0f, 1.0f);
    eyeRay_d = mul_mat_vec(projection_matrix, temp);
    //eyeRay_d.w = 0.0f;

    //eyeRay_d -= eyeRay_o;

    // find intersection with box
    float tnear, tfar;
    int hit = intersectBox(eyeRay_o, eyeRay_d, boxMin, boxMax, &tnear, &tfar);
    if (!hit)
    {
        if ((x < width) && (y < height))
        {
            // return invalid coords
            return (int4)(-1, -1, -1, -1);
        }
    }

    //calculate voxel position
    int4 voxel_position;

    float t = tnear + step;

    //if ray goes out from bounding volume
    if (t > tfar)
    {
        // return invalid coords
        return (int4)(-1, -1, -1, -1);
    }

    float4 pos = eyeRay_o + eyeRay_d*t;
    pos = pos*0.5f+0.5f;    // map position to [0, 1] coordinates

    voxel_position.x = min(max((int)pos.x, 0), (int)dimensions.x-1);          //clamp((int)floor(pos.x), 0, (int)dimensions[0]-1);
    voxel_position.y = min(max((int)pos.y, 0), (int)dimensions.y-1);
    voxel_position.z = min(max((int)pos.z, 0), (int)dimensions.z-1);
    voxel_position.w = 0;

    return voxel_position;
}

__kernel void
inconsistent_voxel_rejection ( __global uchar * hypotheses,
                                __global __const float * bounding_box,
                                __global __const uint * dimensions,
                                __global int * z_buffer,
                                __global float16 * projection_matrices,
                                __global float16 * unprojection_matrices,
                                __global float16 * image_calibration_matrices,
                                uint current_image_number,
                                uint number_of_images,
                                float threshold,
                                float step_size)
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);
    uint width = get_global_size(0);
    uint height = get_global_size(1);

    // calculate offset in z buffer
    __const uint z_buffer_offset = (uint)x +
                                   (uint)y*width +
                                   current_image_number*width*height;

    uint hypotheses_offset = 0;

    int4 voxel_position;

    // if find voxel is -1, didn't find _any_ visible voxels
    // if find voxel is 0 , didn't hit to bounding volume
    // if find_voxel is 1, find visible voxel
    int find_voxel = 0;

    // step to go inside bounding volume
    float step = 0.0f;

    while (find_voxel == 0)
    {
        voxel_position = hit_voxel((float8)(bounding_box[0], bounding_box[1], bounding_box[2], bounding_box[3],
                                            bounding_box[4], bounding_box[5], 0.0f, 0.0f),
                                   x, y, width, height,
                                   (float4)(image_calibration_matrices[current_image_number].s3,
                                            image_calibration_matrices[current_image_number].s7,
                                            image_calibration_matrices[current_image_number].sB,
                                            0.0f),
                                   unprojection_matrices[current_image_number],
                                   (uint4)(dimensions[0], dimensions[1], dimensions[2], 0),
                                   step,
                                   &find_voxel);

        // if we didn't find _any_ visible voxels, stop process this ray
        if (voxel_position.x == -1 && voxel_position.y == -1 &&
            voxel_position.z == -1 && voxel_position.w == -1)
        {
            z_buffer[z_buffer_offset] = -1;
            return;
        }

        // calculate offset to voxel in hypothesis buffer
       hypotheses_offset = voxel_position.x*(1 + number_of_images) +
                           voxel_position.y*dimensions[0]*(1 + number_of_images) +
                           voxel_position.z*dimensions[0]*dimensions[1]*(1 + number_of_images);

        // check is voxel visible
        if (vload4(hypotheses_offset, hypotheses).x == 1)
            find_voxel = 1;                                 // if voxel is visible
        else
        {
            step += step_size;
        }
    }

    //save voxel index
    __const int voxel_index = voxel_position.x + voxel_position.y*dimensions[0] + voxel_position.z*dimensions[0]*dimensions[1];
    z_buffer[z_buffer_offset] = voxel_index;

    uchar4 hypothesis_color = vload4(hypotheses_offset + 1 + current_image_number, hypotheses);

    // if hypothesis is not consist
    if ((hypothesis_color.x + hypothesis_color.y + hypothesis_color.z + hypothesis_color.w) == 0)
        return;

    uint consistent = 0;
    float4 pos_at_second_image;
    uint z_buffer_offset_second;

    float4 voxel_position_3d = (float4)(bounding_box[0] + ((float)voxel_position.x + 0.5f)*bounding_box[3]/(float)dimensions[0],
                                        bounding_box[1] + ((float)voxel_position.y + 0.5f)*bounding_box[4]/(float)dimensions[1],
                                        bounding_box[2] + ((float)voxel_position.z + 0.5f)*bounding_box[5]/(float)dimensions[2],
                                        1.0f);


    for (uint i = 0; i < number_of_images && consistent == 0; ++i)
    {
        pos_at_second_image = position_at_image(projection_matrices[i], voxel_position_3d);
        pos_at_second_image.z = i;

        if (is_in_image(pos_at_second_image, (float4)(0.0f, 0.0f, convert_float(width), convert_float(height))))
            continue;

        z_buffer_offset_second = (uint)floor(pos_at_second_image.x) +
                                 (uint)floor(pos_at_second_image.y)*width +
                                 i*width*height;

        // if hypothesis not occupied and
        // in z buffer we have the same voxel as current and
        // if it is not the same hypothesis
        if (z_buffer[z_buffer_offset_second] == voxel_index &&  i != current_image_number)
        {
            uint current_offset = hypotheses_offset + 1 + i;
            uchar4 color = vload4 (current_offset, hypotheses);

            if (isless(distance(normalize(convert_float4(color)), normalize(convert_float4(hypothesis_color))), threshold))
                consistent = 1;
        }
    }

    // hypothesis is not consistent
    if (!consistent)
        vstore4((uchar4)(0), hypotheses_offset + 1 + current_image_number, hypotheses);
    else
        vstore4(hypothesis_color, hypotheses_offset + 1 + current_image_number, hypotheses);
}
