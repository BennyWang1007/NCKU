#include <iomanip>
#include <iostream>

using namespace std;

int **matrix_chain_order(int *p, int n)
{
    int **s = (int **) malloc(sizeof(int *) * (n + 1));
    int **c = (int **) malloc(sizeof(int *) * (n + 1));
    for (int i = 0; i < n; ++i) {
        s[i] = (int *) calloc(n + 1, sizeof(int));
        c[i] = (int *) calloc(n + 1, sizeof(int));
    }
    for (int i = 0; i < n; ++i) {
        s[i][i] = 0;
    }
    for (int l = 2; l <= n; ++l) {
        for (int i = 1; i < n - l + 1; ++i) {
            int j = i + l - 1;
            s[i][j] = s[i + 1][j] + p[i - 1] * p[i] * p[j];
            for (int k = i; k < j; ++k) {
                int r = s[i][k] + s[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (r < s[i][j]) {
                    s[i][j] = r;
                    c[i][j] = k;
                }
            }
        }
    }
    cout << "c:\n";
    for (int i = 1; i < n; ++i) {
        for (int j = 1; j < n; ++j) {
            cout << right << setw(4) << c[i][j] << " ";
        }
        cout << "\n";
    }
    cout << endl;
    return s;
}


int main()
{
    int p[] = {2, 3, 5, 4, 2};
    int n = sizeof(p) / sizeof(p[0]);
    int **s = matrix_chain_order(p, n);
    for (int i = 1; i < n; ++i) {
        for (int j = 1; j < n; ++j) {
            cout << right << setw(4) << s[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "best: " << s[1][n - 1] << "\n";
    return 0;
}