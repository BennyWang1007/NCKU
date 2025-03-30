#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

int longestAscendingSeq(int *a, int n)
{
    int *table = (int *) malloc(sizeof(int) * n);
    int *prev = (int *) malloc(sizeof(int) * n);
    for (int i = 0; i < n; ++i) {
        table[i] = 1;
        prev[i] = -1;
    }
    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (a[i] > a[j] && table[i] < table[j] + 1) {
                table[i] = table[j] + 1;
                prev[i] = j;
            }
        }
    }
    int result = 0;
    for (int i = 0; i < n; ++i) {
        if (result < table[i]) {
            result = table[i];
        }
    }
    int *resultSeq = (int *) malloc(sizeof(int) * result);
    int index = 0;
    for (int i = 0; i < n; ++i) {
        if (table[i] == result) {
            index = i;
            break;
        }
    }
    for (int i = result - 1; i >= 0; --i) {
        resultSeq[i] = a[index];
        index = prev[index];
    }
    cout << "Longest ascending sequence: ";
    for (int i = 0; i < result; ++i) {
        cout << resultSeq[i] << " ";
    }
    return result;
}


int main()
{
    int a[] = {10, 22, 9, 33, 21, 50, 41, 60, 80};
    int n = sizeof(a) / sizeof(a[0]);
    cout << longestAscendingSeq(a, n) << "\n";
    return 0;
}