# Extracting Matrix A from the Image

1. Save the code-load_image_jpeg.c for jpg format image and load_image_png.c for png format in the same directory where the image is stored. (Change the name of the file according to your image name, in the code file name is given as globe.png and globe.jpg)
2. Run the commands (according to the image format):
   1. gcc load_globe_jpeg.c -o load_globe_jpeg -ljpeg
   2. ./load_globe_jpeg
      (or)
   1. gcc load_globe_png.c -o load_globe_png -lpng -lz
   2. ./load_globe_png
3. This writes the whole matrix into a file named matrix_output.txt and output displayed on the screen gives the size of the matrix which should be noted for further use.

# Implementing truncated SVD and producing the approximate images fo different k values

1. Save main.c, svd.c, matrix_utils.c, matrix_utils.h and svd.h in the smae directory where matrix_output.txt is stored.
2. Run the commands:
   1. gcc main.c svd.c matrix_utils.c -o svd_compress -lm
   2. ./svd_compress <input> <output> <k> <rows> <cols>
       (EX: ./svd_compress matrix_output.txt approx_k20.pgm 20 512 512).
3. This generates the approximate images according to the given k values.