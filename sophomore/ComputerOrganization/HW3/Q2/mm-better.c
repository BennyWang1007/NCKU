# define min(a, b) ((a) < (b) ? (a) : (b))

void matrix_multiplication(int *a, int *b, int *output, int i,
                           int k, int j) {
    int blockSize = 8;
    for (int xx = 0; xx < i; xx += blockSize) {
        for (int yy = 0; yy < j; yy += blockSize) {
            for (int zz = 0; zz < k; zz += blockSize) {
                const int x_end = min(xx + blockSize, i);
                const int y_end = min(yy + blockSize, j);
                const int z_end = min(zz + blockSize, k);
                for (int x = xx; x < x_end; x++) {
                    for (int y = yy; y < y_end; y++) {
                        int sum = 0;
                        for (int z = zz; z < z_end; z++) {
                            sum += a[x * k + z] * b[z * j + y];
                        }
                        output[x * j + y] += sum;
                    }
                }
            }
            
        }
    }
    return;
}