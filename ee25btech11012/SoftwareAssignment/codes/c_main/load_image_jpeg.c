#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

/*from the images given we generate the matrix of pixel intensities and write it into a file named matrix_output.txt for further use, using this code. */

int main() {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile;
    char *filename = "globe.jpg";
    int width_n, height_m;
    unsigned char **A;
    cinfo.err = jpeg_std_error(&jerr);
    if ((infile = fopen(filename, "rb"))  ==NULL) {
        fprintf(stderr, "unable to open %s\n", filename);
        return 1;
    }
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_GRAYSCALE;
    jpeg_start_decompress(&cinfo);
    width_n = cinfo.output_width;
    height_m = cinfo.output_height;
    printf("Image loaded through libjpeg: %s\n", filename);
    printf("Width (size) = %d pixels\n", width_n);
    printf("Height (end) = %d pixels\n", height_m);

    A = (unsigned char **)malloc(height_m * sizeof(unsigned char *));
    if (A  ==NULL) {  }
    for (int r = 0; r < height_m; r++) {
        A[r] = (unsigned char *)malloc(width_n * sizeof(unsigned char));
        if (A[r]  ==NULL) {  }
    }
    while (cinfo.output_scanline < cinfo.output_height) {
        int r = cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, &A[r], 1);
    }
    printf("Data copied to 2D matrix 'A'.\n");

    FILE *outFile = fopen("matrix_output.txt", "w");
    if (outFile  ==NULL) {
        fprintf(stderr, "unable to open output file for writing.\n");

        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        for (int r = 0; r < height_m; r++) free(A[r]);
        free(A);
        return 1;
    }
    printf("\nWriting full %d data %d matrix to 'matrix_output.txt'...\n", height_m, width_n);

    for (int r = 0; r < height_m; r++) {
        for (int c = 0; c < width_n; c++) {

            fprintf(outFile, "%3d ", A[r][c]);
        }

        fprintf(outFile, "\n");
    }

    fclose(outFile);
    printf("Done.\n");
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    for (int r = 0; r < height_m; r++) {
        free(A[r]);}
    free(A);
    return 0;
}