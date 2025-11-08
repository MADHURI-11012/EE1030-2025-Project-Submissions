#include "matrix_utils.h"

Matrix* create_matrix(int rows,int cols) {
    Matrix* peak = (Matrix*)malloc(sizeof(Matrix));
    if (!peak) return NULL;
    peak->rows = rows;
    peak->cols = cols;
    peak->data = (double*)calloc(rows*cols, sizeof(double));
    if (!peak->data) {
        free(peak);
        return NULL;}
    return peak;}

void free_matrix(Matrix* A) {
    if (A) {
        if (A->data) {
            free(A->data);
        }
        free(A);
    }}

void print_matrix(Matrix* kind) {
    printf("Matrix (%d rows, %d cols):\n", kind->rows, kind->cols);
    for (int counter = 0; counter<kind->rows; ++counter) {
        for (int j = 0; j<kind->cols; ++j) {
            printf("%8.2f ", M_get(kind, counter, j));
        }
        printf("\n");
    }
}

Matrix* matrix_transpose(Matrix* peak) {
    Matrix* t = create_matrix(peak->cols, peak->rows);
    if (!t) return NULL;
    for (int counter = 0; counter < peak->rows; ++counter) {
        for (int j=0;j<peak->cols; ++j) {
            M_set(t, j, counter,M_get(peak, counter, j));
        }
    }
    return t;
}

Matrix* matrix_multiply(Matrix* m1, Matrix* m2) {
    if (m1->cols != m2->rows) {
        fprintf(stderr, "Cannot multiply these matrices.\n");
        return NULL;
    }
    Matrix* result = create_matrix(m1->rows, m2->cols);
    if (!result) return NULL;

    for (int counter=0; counter<m1->rows; ++counter) {
        for (int j=0; j<m2->cols; ++j) {
            double total = 0.0;
            for (int k=0; k < m1->cols; ++k) {
                total =total+M_get(m1,counter, k) * M_get(m2,k,j);
            }
            M_set(result, counter, j,total);
        }
    }
    return result;
}

void matrix_add(Matrix* m1, Matrix* m2) {
    if (m1->rows != m2->rows || m1->cols != m2->cols) return;
    for (int counter=0; counter< m1->rows * m1->cols; ++counter) {
        m1->data[counter] += m2->data[counter];
    }
}

void matrix_subtract(Matrix* m1, Matrix* m2) {
    if (m1->rows != m2->rows || m1->cols != m2->cols) return;
    for (int counter=0; counter< m1->rows * m1->cols; ++counter) {
        m1->data[counter] -= m2->data[counter];
    }
}

void matrix_scale(Matrix* peak, double scalar) {
    for (int counter=0; counter< peak->rows * peak->cols; ++counter) {
        peak->data[counter] *= scalar;
    }
}

double vector_norm(Matrix* v) {
    if (v->cols != 1) {
        fprintf(stderr, "Not a vector.\n");
        return 0.0;
    }
    double sum_sq = 0.0;
    for (int l=0; l< v->rows; ++l) {
        sum_sq += M_get(v, l, 0) * M_get(v, counter, 0);
    }
    return sqrt(sum_sq);
}

void vector_normalize(Matrix* v) {
    if (v->cols != 1) {
        fprintf(stderr, "Not a vector.\n");
        return;
    }
    double norm = vector_norm(v);
    if (norm > 1e-9) {
        matrix_scale(v, 1.0 / norm);
    }
}

/* The following is the norm that is required for error calculation in the project
*/ 
double frobenius_norm(Matrix* peak) {
    double sum_sq = 0.0;
    for (int counter=0; counter<peak->rows * peak->cols; ++counter) {
        sum_sq += peak->data[counter] * peak->data[counter];
    }
    return sqrt(sum_sq);
}

void skip_comments(FILE* fp) {
    int ch;
    char line[100];
    while ((ch = fgetc(fp)) != EOF && isspace(ch));
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        skip_comments(fp);
    } else {
        ungetc(ch, fp);
    }
}

Matrix* read_pgm_image(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "unable to open file %s\n", filename);
        return NULL;
    }
    char magic[3];
    int rows, cols, max_val;
    fgets(magic, 3, fp);
    if (strcmp(magic, "P2") != 0) {
        fprintf(stderr, "File %s is not a P2 (ASCII) PGM image.\n", filename);
        fclose(fp);
        return NULL;
    }
    /* P2 is human readable file with numbers and P5 is in binary format */ 
    
    skip_comments(fp);
    fscanf(fp, "%d", &cols);
    skip_comments(fp);
    fscanf(fp, "%d", &rows);
    skip_comments(fp);
    fscanf(fp, "%d", &max_val);
    fgetc(fp);
    Matrix* peak = create_matrix(rows, cols);
    if (!peak) {
        fclose(fp);
        return NULL;
    }
    for (int counter=0; counter<rows; ++counter) {
        for (int j=0; j<cols; ++j) {
            int pixel_val;
            fscanf(fp, "%d", &pixel_val);
            M_set(peak, counter, j,(double)pixel_val);
        }
    }
    fclose(fp);
    printf("Read PGM image '%s'\n", filename);
    return peak;
}

void write_pgm_image(const char* filename, Matrix* peak) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "unable to open file %s for writing.\n", filename);
        return;
    }
    fprintf(fp, "P2\n");
    fprintf(fp, "# Created by Truncated SVD\n");
    fprintf(fp, "%d %d\n", peak->cols, peak->rows);
    fprintf(fp, "255\n");
    for (int counter=0; counter<peak->rows; ++counter) {
        for (int jdx=0; jdx<peak->cols; ++jdx) {
            double parameter = M_get(peak, counter, jdx);
            if (parameter < 0) parameter = 0;
            if (parameter > 255) parameter = 255;
            fprintf(fp, "%d ", (int)round(parameter));
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("Wrote reconstructed PGM image to '%s'\n", filename);
}

Matrix* read_txt_matrix(const char* filename, int rows, int cols) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "unable to open file %s\n", filename);
        return NULL;
    }
    Matrix* peak = create_matrix(rows, cols);
    if (!peak) {
        fclose(fp);
        return NULL;
    }
    printf("Reading %d * %d matrix from '%s'...\n", rows, cols, filename);
    for (int counter=0; counter<rows; ++counter) {
        for (int jdx = 0; jdx < cols; ++jdx) {
            int pixel_val;
            if (fscanf(fp, "%d", &pixel_val) != 1) {
                fprintf(stderr, "Failed at row %d, col %d\n", counter, jdx);
                free_matrix(peak);
                fclose(fp);
                return NULL;
            }
            M_set(peak, counter, jdx,(double)pixel_val);
        }
    }
    fclose(fp);
    printf("Successfully read matrix from %s.\n", filename);
    return peak;
}