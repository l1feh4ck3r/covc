void inverse(const float * matrix, float * inverted_matrix);
void multiply_matrix_vector(const float * matrix, const float * vector, float * result);
void multiply_vector_vector(const float * vec1, const float *vec2, float * result);
void normalize_vector(const float * vector, float * result);
float vector_length(const float * vector);
void vector_minus_vector(const float * vec1, const float * vec2, float * result);
void vector_plus_vector(const float * vec1, const float * vec2, float * result);
