#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

enum Direction { NONE, UP, LEFT, UP_LEFT };

auto LCS(char *X, char *Y)
{
    int m = sizeof(X) / sizeof(X[0]);
    int n = sizeof(Y) / sizeof(Y[0]);
    int **c = (int **) malloc(sizeof(int *) * (m + 1));
    int **b = (int **) malloc(sizeof(int *) * (m + 1));
    for (int i = 0; i < m + 1; ++i) {
        c[i] = (int *) calloc(n + 1, sizeof(int));
        b[i] = (int *) calloc(n + 1, sizeof(int));
    }
    for (int i = 1; i < m + 1; ++i) {
        for (int j = 1; j < n + 1; ++j) {
            if (X[i - 1] == Y[j - 1]) {
                c[i][j] = c[i - 1][j - 1] + 1;
                b[i][j] = UP_LEFT;
            } else if (c[i - 1][j] >= c[i][j - 1]) {
                c[i][j] = c[i - 1][j];
                b[i][j] = UP;
            } else {
                c[i][j] = c[i][j - 1];
                b[i][j] = LEFT;
            }
        }
    }
    return make_pair(c, b);
}


int main()
{
    string x = "GTCA", y = "GTACG";
    char X[] = {'G', 'T', 'C', 'A'};
    char Y[] = {'G', 'T', 'A', 'C', 'G'};
    pair<int **, int **> result = LCS(X, Y);
    int **c = result.first;
    int **b = result.second;
    int m = sizeof(X) / sizeof(X[0]);
    int n = sizeof(Y) / sizeof(Y[0]);
    cout << "c:\n";
    for (int i = 0; i < m + 1; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            cout << right << setw(3) << c[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    cout << "b:\n";
    for (int i = 0; i < m + 1; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            switch (b[i][j]) {
            case UP:
                cout << "UP";
                break;
            case LEFT:
                cout << "LT";
                break;
            case UP_LEFT:
                cout << "LU";
                break;
            default:
                cout << "XX";
                break;
            }
            cout << " ";
        }
        cout << "\n";
    }
    return 0;
}