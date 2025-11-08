# Project Oerview:

1. Use C libraries like jpeglib.h etc, Extract the Matrix of pixel intensities from the given pictures.
2. Store the Matrix(A) into file named matrix_output.txt.
3. Apply truncated SVD to matrix A to produce low-rank approximation matrix A_k.(I am using Power Iteration Algorithm for this)
4. for different values of k, Producing the new approximate images (named in the format approx_k20.pgm etc).
5. Finding the Frobenius norm.(Approximation of error)

# Observation:
1. As the value of k increases image quality increases.
2. As the value of k decreases image compression increases.
