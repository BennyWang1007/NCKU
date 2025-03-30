#include <iomanip>
#include <iostream>

using namespace std;

int knapsack(int n, int v[], int w[], int W)
{
    int **table = (int **) malloc(sizeof(int *) * (n + 1));
    for (int i = 0; i < n + 1; ++i) {
        table[i] = (int *) malloc(sizeof(int) * (W + 1));
    }
    for (int i = 0; i < W + 1; ++i) {
        table[0][i] = 0;
    }
    for (int i = 1; i < n + 1; ++i) {
        table[i][0] = 0;
        for (int j = 1; j < W + 1; ++j) {
            if (w[i] <= j) {
                table[i][j] =
                    max(table[i - 1][j], v[i] + table[i - 1][j - w[i]]);
            } else {
                table[i][j] = table[i - 1][j];
            }
        }
    }
    // print table
    cout << " s: ";
    for (int i = 0; i < W + 1; ++i) {
        cout << right << setw(2) << i << " ";
    }
    cout << "\n";
    for (int i = 0; i < n + 1; ++i) {
        cout << right << setw(2) << i << ": ";
        for (int j = 0; j < W + 1; ++j) {
            cout << right << setw(2) << table[i][j] << " ";
        }
        cout << "\n";
    }
    return table[n][W];
}


int main()
{
    int n = 7;
    int v[] = {0, 8, 6, 15, 3, 2, 5, 9};
    int w[] = {0, 2, 3, 5, 4, 3, 2, 6};
    // int v[] = {0, 5, 8, 6, 15, 3, 2, 9};
    // int w[] = {0, 2, 2, 3, 5, 4, 3, 6};
    int W = 16;
    cout << knapsack(n, v, w, W) << "\n";
    return 0;
}