#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

auto minimumEditDistance(int *a, int *b, int m, int n)
{
    // insertion, deletion, substitution = 1, 2, 3
    int **d = (int **) malloc(sizeof(int *) * (m + 1));
    for (int i = 0; i < m + 1; ++i) {
        d[i] = (int *) calloc(n + 1, sizeof(int));
    }
    for (int i = 0; i < m + 1; ++i) {
        d[i][0] = i * 2;
    }
    for (int j = 0; j < n + 1; ++j) {
        d[0][j] = j * 1;
    }
    for (int i = 1; i < m + 1; ++i) {
        for (int j = 1; j < n + 1; ++j) {
            int insertion = d[i][j - 1] + 1;
            int deletion = d[i - 1][j] + 2;
            int substitution = d[i - 1][j - 1] + 3;
            if (a[i - 1] == b[j - 1]) {
                d[i][j] = d[i - 1][j - 1];
            } else {
                d[i][j] = min(insertion, min(deletion, substitution));
            }
        }
    }

    return d;
}


int main()
{
    int a[] = {1, 2, 5, 4, 5};
    int b[] = {1, 2, 3, 4, 5};
    int m = sizeof(a) / sizeof(a[0]);
    int n = sizeof(b) / sizeof(b[0]);
    int **d = minimumEditDistance(a, b, m, n);
    cout << "d:\n";
    for (int i = 0; i < m + 1; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            cout << right << setw(3) << d[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "minimum edit distance: " << d[m][n] << "\n";
    return 0;
}