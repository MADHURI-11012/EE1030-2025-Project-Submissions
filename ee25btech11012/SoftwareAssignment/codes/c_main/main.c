#include <stdio.h>
#include "matrix_utils.h"
#include "svd.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <input_txt> <output_pgm> <k_value> <rows> <cols>\n", argv[0]);
        return 1;
    }
    const char* input_file = argv[1];
    const char* output_file = argv[2];

/* atoi() (stands for ASCII to integer)is a function in C that converts a string (text) containing numbers into an integer value.*/

    int k_valu = atoi(argv[3]);
    int rows = atoi(argv[4]);
    int cols = atoi(argv[5]);

    if (k_valu <= 0 || rows <= 0 || cols <= 0) {
        fprintf(stderr, "k_value,rows and cols must be positive integers.\n");
        return 1;
    }
    Matrix* A = read_txt_matrix(input_file, rows, cols);
    if (!A) {
        fprintf(stderr, "Failed to read input matrix.\n");
        return 1;
    }

    Matrix* A_k = compute_svd(A, k_valu);
    if (!A_k) {
         fprintf(stderr, "SVD failed.\n");
         free_matrix(A);
         return 1;
    }

/* finding the error */
    Matrix* diff = create_matrix(A->rows, A->cols);

/* memcpy(A,B,n) copies n bytes from B to A */

    memcpy(diff->data, A->data, A->rows * A->cols * sizeof(double));
    matrix_subtract(diff, A_k);
    double error = frobenius_norm(diff);
    double original_norm = frobenius_norm(A);

    printf("Frobenius Norm of Original (||A||_F): %.4f\n", original_norm);
    printf("Frobenius Norm of Reconstructed (||A_k||_F): %.4f\n", frobenius_norm(A_k));
    printf("Frobenius Norm of Error (||A - A_k||_F): %.4f\n", error);
    printf("Relative Error: %.4f%%\n", (error / original_norm) * 100.0);

    write_pgm_image(output_file, A_k);

    free_matrix(A);
    free_matrix(A_k);
    free_matrix(diff);

    return 0;
}