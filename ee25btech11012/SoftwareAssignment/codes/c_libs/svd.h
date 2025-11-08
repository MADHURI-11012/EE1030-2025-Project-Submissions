#ifndef SVD_H
#define SVD_H
/* to prevent the information of headerfile to be included again and again
*/

#include "matrix_utils.h"
#define POWER_ITERATIONS 10 

Matrix* compute_svd(Matrix* A, int k);
#endif