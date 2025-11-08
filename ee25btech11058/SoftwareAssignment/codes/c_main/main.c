#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../c_libs/stb_image.h"
#include "../c_libs/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

double dotp(const double *a, const double *b, int n) {
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += a[i] * b[i];
    return s;
}

void multimatvec(const double *A, int r, int c, const double *x, double *y) {
    for (int i = 0; i < r; i++) {
        double t = 0.0;
        for (int j = 0; j < c; j++)
            t += A[i * c + j] * x[j];
        y[i] = t;
    }
}

void multitransvec(const double *A, int r, int c, const double *x, double *y) {
    for (int j = 0; j < c; j++) {
        double t = 0.0;
        for (int i = 0; i < r; i++)
            t += A[i * c + j] * x[i];
        y[j] = t;
    }
}

void powiter(const double *A, int r, int c, double *vout, double *alphaout) {
    double *v = malloc((size_t)c * sizeof(double));
    double *vnext = malloc((size_t)c * sizeof(double));
    double *temp = malloc((size_t)r * sizeof(double));

    for (int i = 0; i < c; i++)
        v[i] = 1.0;

    double alpha = 0.0;

    for (int it = 0; it < 1000; it++) {
        multimatvec(A, r, c, v, temp);
        multitransvec(A, r, c, temp, vnext);

        double norm = sqrt(dotp(vnext, vnext, c));
        if (norm == 0.0)
            break;

        for (int i = 0; i < c; i++)
            vnext[i] /= norm;

        double newa = dotp(v, vnext, c);
        if (fabs(newa - alpha) < 1e-9)
            break;

        alpha = newa;
        for (int i = 0; i < c; i++)
            v[i] = vnext[i];
    }

    for (int i = 0; i < c; i++)
        vout[i] = v[i];

    *alphaout = alpha;

    free(v);
    free(vnext);
    free(temp);
}

void savesvdcom(const unsigned char *img, int h, int w, int k, const char *base) {
    int rows = h, cols = w;

    double *A = malloc((size_t)rows * cols * sizeof(double));
    double *Ao = malloc((size_t)rows * cols * sizeof(double));

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            double val = img[i * cols + j];
            A[i * cols + j] = val;
            Ao[i * cols + j] = val;
        }

    if (k > (rows < cols ? rows : cols))
        k = (rows < cols ? rows : cols);

    double *S = calloc((size_t)k, sizeof(double));
    double **U = malloc((size_t)k * sizeof(double *));
    double **V = malloc((size_t)k * sizeof(double *));

    for (int i = 0; i < k; i++) {
        U[i] = calloc((size_t)rows, sizeof(double));
        V[i] = calloc((size_t)cols, sizeof(double));
    }

    double *v = malloc((size_t)cols * sizeof(double));
    double *temp = malloc((size_t)rows * sizeof(double));

    for (int c = 0; c < k; c++) {
        double a;
        powiter(A, rows, cols, v, &a);
        double s = sqrt(a);
        S[c] = s;

        multimatvec(A, rows, cols, v, temp);

        for (int i = 0; i < rows; i++)
            U[c][i] = (s == 0.0) ? 0.0 : temp[i] / s;

        for (int j = 0; j < cols; j++)
            V[c][j] = v[j];

        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                A[i * cols + j] -= s * U[c][i] * V[c][j];
    }

    double *Recon = calloc((size_t)rows * cols, sizeof(double));

    for (int c = 0; c < k; c++) {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                Recon[i * cols + j] += S[c] * U[c][i] * V[c][j];
    }

    double frob = 0.0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            double diff = Ao[i * cols + j] - Recon[i * cols + j];
            frob += diff * diff;
        }

    frob = sqrt(frob);

    unsigned char *Out = malloc((size_t)rows * cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            double val = Recon[i * cols + j];
            if (val < 0.0) val = 0.0;
            if (val > 255.0) val = 255.0;
            Out[i * cols + j] = (unsigned char)(val + 0.5);
        }

    char filename[512];
    int pos = 0;

    const char *path = "../../figure/output/";
    while (path[pos] != '\0') {
        filename[pos] = path[pos];
        pos++;
    }

    int b = 0;
    while (base[b] != '\0') {
        filename[pos++] = base[b++];
    }

    filename[pos++] = 'k';

    if (k >= 100) {
        filename[pos++] = (k / 100) % 10 + '0';
        filename[pos++] = (k / 10) % 10 + '0';
        filename[pos++] = k % 10 + '0';
    } else if (k >= 10) {
        filename[pos++] = (k / 10) % 10 + '0';
        filename[pos++] = k % 10 + '0';
    } else {
        filename[pos++] = k + '0';
    }

    filename[pos++] = '.';
    filename[pos++] = 'p';
    filename[pos++] = 'n';
    filename[pos++] = 'g';
    filename[pos] = '\0';

    stbi_write_png(filename, cols, rows, 1, Out, cols);

    printf("Saved %s  (k=%d)  FrobeniusError: %.6f\n", filename, k, frob);

    for (int i = 0; i < k; i++) {
        free(U[i]);
        free(V[i]);
    }

    free(U);
    free(V);
    free(S);
    free(A);
    free(Ao);
    free(Recon);
    free(Out);
    free(v);
    free(temp);
}

int process_one_file(const char *file) {
    if (!file)
        return 0;

    char path[512];
    snprintf(path, sizeof(path), "../../figure/input/%s", file);

    int w, h, ch;
    unsigned char *img = stbi_load(path, &w, &h, &ch, 1);
    if (!img) {
        printf("Can't load %s\n", path);
        return 0;
    }

    int kvals[5] = {5, 10, 20, 50, 100};
    char base[128];
    int b = 0;

    while (file[b] && file[b] != '.') {
        base[b] = file[b];
        b++;
    }
    base[b] = '\0';

    for (int i = 0; i < 5; i++)
        savesvdcom(img, h, w, kvals[i], base);

    stbi_image_free(img);
    return 1;
}

int main(int argc, char **argv) {
    const char *defaults[] = {"image1.png", "image2.jpg", "image3.png"};
    int n = 3;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            printf("\nProcessing %s ...\n", argv[i]);
            process_one_file(argv[i]);
        }
    } else {
        for (int i = 0; i < n; i++) {
            printf("\nProcessing %s ...\n", defaults[i]);
            process_one_file(defaults[i]);
        }
    }

    printf("\nAll files processed. Check ../../figure/output/\n");
    return 0;
}
