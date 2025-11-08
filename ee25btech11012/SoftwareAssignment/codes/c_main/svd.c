#include "matrix_utils.h"
#include "svd.h"

 Matrix* compute_svd(Matrix* A, int key) {
 srand(time(NULL));

Matrix* A_k = create_matrix(A->rows, A->cols);

Matrix* A_temp = create_matrix(A->rows, A->cols);
    memcpy(A_temp->data, A->data, A->rows * A->cols * sizeof(double));

 for (int counter = 0; counter < key; ++counter) {
printf("Computing singular value %d/%d...\n", counter + 1, key);

Matrix* v = create_matrix(A_temp->cols, 1);
        for (int step = 0; step < v->rows; ++step) {
        M_set(v, step, 0, (double)rand() / (double)RAND_MAX);
        }
vector_normalize(v);
 Matrix* A_T = matrix_transpose(A_temp);
Matrix* A_T_A = matrix_multiply(A_T, A_temp);

 for (int counter = 0; counter < POWER_ITERATIONS; ++counter) {
        Matrix* v_new = matrix_multiply(A_T_A, v);
        vector_normalize(v_new);
            free_matrix(v);
            v = v_new;
            }

Matrix* u = matrix_multiply(A_temp, v);
double sigma = vector_norm(u);

        vector_normalize(u);
        Matrix* vT = matrix_transpose(v);
        Matrix* u_sigma = create_matrix(u->rows, u->cols);

/* memcpy(A,B,n) copies n bytes from B to A */

        memcpy(u_sigma->data, u->data, u->rows * u->cols * sizeof(double));
        matrix_scale(u_sigma, sigma);
Matrix* rank_1_component = matrix_multiply(u_sigma, vT);

        matrix_add(A_k, rank_1_component);
        matrix_subtract(A_temp, rank_1_component);

free_matrix(A_T);
        free_matrix(A_T_A);
        free_matrix(v);
        free_matrix(u);
        free_matrix(vT);
        free_matrix(u_sigma);
        free_matrix(rank_1_component);
        }

    printf("SVD computation finished.\n");
    free_matrix(A_temp);

    return A_k;
    }