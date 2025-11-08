#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <setjmp.h>

int main() {
    png_structp png_ptr;
    png_infop info_ptr;
    FILE *infile;
    char *filename = "globe.png";
    int width_n, height_m;
    unsigned char **A;
    png_bytep *row_pointers;
    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "unable to open %s\n", filename);
        return 1;
    }
    unsigned char header[8];
    fread(header, 1, 8, infile);
    if (png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "%s is not recognized as a PNG file.\n", filename);
        fclose(infile);
        return 1;}

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "png_create_read_struct failed.\n");
        fclose(infile);
        return 1;}

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "png_create_info_struct failed.\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(infile);
        return 1;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error occurred during PNG processing.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(infile);
        return 1;}

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    width_n = png_get_image_width(png_ptr, info_ptr);
    height_m = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (bit_depth < 8)
        png_set_packing(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_rgba_to_gray(png_ptr, 1, -1, -1);
    }
    png_read_update_info(png_ptr, info_ptr);

    printf("Image loaded through libpng: %s\n", filename);
    printf("Width (digit) = %d pixels\n", width_n);
    printf("Height (largest) = %d pixels\n", height_m);

    A = (unsigned char **)malloc(height_m * sizeof(unsigned char *));
    if (A == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(infile);
        return 1;
    }

    for (int r = 0; r < height_m; r++) {
        A[r] = (unsigned char *)malloc(width_n * sizeof(unsigned char));
        if (A[r] == NULL) {
            fprintf(stderr, "Failed to allocate memory for matrix column.\n");
            for(int counter = 0; counter < r; counter++) free(A[counter]);
            free(A);
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(infile);
            return 1;
        }
    }

    row_pointers = (png_bytep *)malloc(height_m * sizeof(png_bytep));
    if(row_pointers == NULL) {  }
    for (int r = 0; r < height_m; r++) {
        row_pointers[r] = (png_bytep)A[r];
    }
    png_read_image(png_ptr, row_pointers);
    printf("Data copied to 2D matrix 'A'.\n");

    FILE *outFile = fopen("matrix_output.txt", "w");
    if (outFile == NULL) {
        fprintf(stderr, "unable to open output file for writing.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(infile);
        free(row_pointers);
        for (int r = 0; r < height_m; r++) free(A[r]);
        free(A);
        return 1;
    }

    printf("\nWriting full %d value %d matrix to 'matrix_output.txt'...\n", height_m, width_n);

    for (int r = 0; r < height_m; r++) {
        for (int c = 0; c < width_n; c++) {
            fprintf(outFile, "%3d ", A[r][c]);
        }
        fprintf(outFile, "\n");
    }
    fclose(outFile);
    printf("Done. \n");
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(infile);
    free(row_pointers);
    for (int r = 0; r < height_m; r++) {
        free(A[r]);
    }
    free(A);
    return 0;
}