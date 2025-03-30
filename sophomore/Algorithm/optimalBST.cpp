#include <cstdlib>
#include <iomanip>
#include <iostream>

using namespace std;

pair<int **, pair<int **, int **>> optimal_BST(int *p, int *q, int n)
{
    int **e, **w, **root;
    e = (int **) malloc(sizeof(int *) * (n + 1));
    w = (int **) malloc(sizeof(int *) * (n + 1));
    root = (int **) malloc(sizeof(int *) * n);
    for (int i = 0; i < n + 1; ++i) {
        e[i] = (int *) calloc(n + 1, sizeof(int));
        w[i] = (int *) calloc(n + 1, sizeof(int));
    }
    for (int i = 0; i < n; ++i) {
        root[i] = (int *) calloc(n, sizeof(int));
    }
    for (int i = 0; i < n + 1; ++i) {
        e[i][i] = q[i];
        w[i][i] = q[i];
    }
    for (int l = 1; l < n + 1; ++l) {
        for (int i = 0; i < n - l + 1; ++i) {
            int j = i + l;
            e[i][j] = INT_MAX;
            w[i][j] = w[i][j - 1] + p[j] + q[j];
            for (int r = i; r < j; ++r) {
                int t = e[i][r] + e[r + 1][j] + w[i][j];
                if (t < e[i][j]) {
                    e[i][j] = t;
                    root[i][j - 1] = r;
                }
            }
        }
    }
    return make_pair(e, make_pair(w, root));
}


int main()
{
    int p[] = {0, 5, 2, 4, 3};
    int q[] = {3, 2, 3, 4, 2};
    int n = sizeof(p) / sizeof(p[0]) - 1;
    pair<int **, pair<int **, int **>> result = optimal_BST(p, q, n);
    int **e = result.first;
    int **w = result.second.first;
    int **root = result.second.second;

    cout << "e:\n";
    for (int i = 0; i < n + 1; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            cout << right << setw(4) << e[i][j] << " ";
        }
        cout << "\n";
    }

    cout << "\nw:\n";
    for (int i = 0; i < n + 1; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            cout << right << setw(4) << w[i][j] << " ";
        }
        cout << "\n";
    }

    cout << "\nroot:\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << right << setw(4) << root[i][j] << " ";
        }
        cout << "\n";
    }

    return 0;
}
