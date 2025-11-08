#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H
/* to prevent the information of headerfile to be included again and again
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
/* will use this in svd.c for producing new vector each and every time*/

typedef struct {
    int rows;
    int cols;
    double* data;
} Matrix;

/* in compiler 2d array is stored as 1d so to find the position*/

static inline double M_get(const Matrix* m, int r, int c) {
    return m->data[r * m->cols + c];
}

static inline void M_set(Matrix* m, int r, int c, double value) {
    m->data[r * m->cols + c] = value;
}

Matrix* create_matrix(int rows, int cols);
Matrix* matrix_transpose(Matrix* A);
Matrix* matrix_multiply(Matrix* m1, Matrix* m2);
void matrix_add(Matrix* m1, Matrix* m2);
void matrix_subtract(Matrix* m1, Matrix* m2);
void matrix_scale(Matrix* B, double scalar);
double vector_norm(Matrix* v);
void vector_normalize(Matrix* v);
double frobenius_norm(Matrix* H);
void free_matrix(Matrix* P);
void print_matrix(Matrix* C);

Matrix* read_pgm_image(const char* filename);
void write_pgm_image(const char* filename, Matrix* largest);
Matrix* read_txt_matrix(const char* filename, int rows, int cols);

#endif