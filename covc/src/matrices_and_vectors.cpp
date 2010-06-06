#include "matrices_and_vectors.h"

#include <stdlib.h>
#include <math.h>

void inverse(const float *matrix, float *inverted_matrix)
{
    float a0 = matrix[ 0]*matrix[ 5] - matrix[ 1]*matrix[ 4];
    float a1 = matrix[ 0]*matrix[ 6] - matrix[ 2]*matrix[ 4];
    float a2 = matrix[ 0]*matrix[ 7] - matrix[ 3]*matrix[ 4];
    float a3 = matrix[ 1]*matrix[ 6] - matrix[ 2]*matrix[ 5];
    float a4 = matrix[ 1]*matrix[ 7] - matrix[ 3]*matrix[ 5];
    float a5 = matrix[ 2]*matrix[ 7] - matrix[ 3]*matrix[ 6];
    float b0 = matrix[ 8]*matrix[13] - matrix[ 9]*matrix[12];
    float b1 = matrix[ 8]*matrix[14] - matrix[10]*matrix[12];
    float b2 = matrix[ 8]*matrix[15] - matrix[11]*matrix[12];
    float b3 = matrix[ 9]*matrix[14] - matrix[10]*matrix[13];
    float b4 = matrix[ 9]*matrix[15] - matrix[11]*matrix[13];
    float b5 = matrix[10]*matrix[15] - matrix[11]*matrix[14];

    float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    float epsilon = 0.1;
    if (fabs(det) > epsilon)
    {
        float inverse[16];

        inverse[ 0] = + matrix[ 5]*b5 - matrix[ 6]*b4 + matrix[ 7]*b3;
        inverse[ 4] = - matrix[ 4]*b5 + matrix[ 6]*b2 - matrix[ 7]*b1;
        inverse[ 8] = + matrix[ 4]*b4 - matrix[ 5]*b2 + matrix[ 7]*b0;
        inverse[12] = - matrix[ 4]*b3 + matrix[ 5]*b1 - matrix[ 6]*b0;
        inverse[ 1] = - matrix[ 1]*b5 + matrix[ 2]*b4 - matrix[ 3]*b3;
        inverse[ 5] = + matrix[ 0]*b5 - matrix[ 2]*b2 + matrix[ 3]*b1;
        inverse[ 9] = - matrix[ 0]*b4 + matrix[ 1]*b2 - matrix[ 3]*b0;
        inverse[13] = + matrix[ 0]*b3 - matrix[ 1]*b1 + matrix[ 2]*b0;
        inverse[ 2] = + matrix[13]*a5 - matrix[14]*a4 + matrix[15]*a3;
        inverse[ 6] = - matrix[12]*a5 + matrix[14]*a2 - matrix[15]*a1;
        inverse[10] = + matrix[12]*a4 - matrix[13]*a2 + matrix[15]*a0;
        inverse[14] = - matrix[12]*a3 + matrix[13]*a1 - matrix[14]*a0;
        inverse[ 3] = - matrix[ 9]*a5 + matrix[10]*a4 - matrix[11]*a3;
        inverse[ 7] = + matrix[ 8]*a5 - matrix[10]*a2 + matrix[11]*a1;
        inverse[11] = - matrix[ 8]*a4 + matrix[ 9]*a2 - matrix[11]*a0;
        inverse[15] = + matrix[ 8]*a3 - matrix[ 9]*a1 + matrix[10]*a0;

        float invDet = ((float)1)/det;
        inverse[ 0] *= invDet;
        inverse[ 1] *= invDet;
        inverse[ 2] *= invDet;
        inverse[ 3] *= invDet;
        inverse[ 4] *= invDet;
        inverse[ 5] *= invDet;
        inverse[ 6] *= invDet;
        inverse[ 7] *= invDet;
        inverse[ 8] *= invDet;
        inverse[ 9] *= invDet;
        inverse[10] *= invDet;
        inverse[11] *= invDet;
        inverse[12] *= invDet;
        inverse[13] *= invDet;
        inverse[14] *= invDet;
        inverse[15] *= invDet;

        for (size_t i = 0; i < 16; ++i)
            inverted_matrix[i] = inverse[i];
    }
}

void multiply_matrix_vector(const float *matrix, const float *vector, float *result)
{
    float temp[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            temp[i] += matrix[i*4 + j]*vector[j];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}

void multiply_vector_vector(const float *vec1, const float *vec2, float *result)
{
    float temp[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    temp[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
    temp[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
    temp[2] = vec1[0]*vec2[1] - vec1[1]*vec2[0];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}

void normalize_vector(const float *vector, float *result)
{
    float sum = 0.0f;
    for (size_t i = 0; i < 4; ++i)
        sum += vector[i]*vector[i];

    float radix = sqrt(sum);
    for (size_t i = 0; i < 4; ++i)
        result[i] = vector[i] / radix;
}

float vector_length(const float * vector)
{
    return sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2] + vector[3]*vector[3]);
}

void vector_minus_vector(const float *vec1, const float *vec2, float *result)
{
    float temp[4];
    for (size_t i = 0; i < 4; ++i)
        temp[i] = vec1[i] - vec2[i];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}

void vector_plus_vector(const float *vec1, const float *vec2, float *result)
{
    float temp[4];
    for (size_t i = 0; i < 4; ++i)
        temp[i] = vec1[i] + vec2[i];

    for (size_t i = 0; i < 4; ++i)
        result[i] = temp[i];
}
