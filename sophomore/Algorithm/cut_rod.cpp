#include <cstdlib>
#include <iostream>
#include <limits>

using namespace std;

int memorized_cut_aux(int *price, int n, int *table)
{
    if (table[n] >= 0)
        return table[n];
    if (n == 0) {
        table[0] = 0;
        return 0;
    }
    int q = INT_MIN;
    for (int i = 1; i < n + 1; ++i) {
        q = max(q, price[i] + memorized_cut_aux(price, n - i, table));
    }
    table[n] = q;
    return q;
}

int memorized_cut(int *price, int n)
{
    int *table = (int *) malloc(sizeof(int) * (n + 1));
    for (int i = 0; i < n + 1; ++i) {
        table[i] = INT_MIN;
    }
    int result = memorized_cut_aux(price, n, table);
    // for (int i = 0; i < n + 1; ++i) {
    //     cout << i << ": " << table[i] << "\n";
    // }
    return result;
}

int buttom_up_cut(int *price, int n)
{
    int *table = (int *) malloc(sizeof(int) * (n + 1));
    table[0] = 0;
    for (int i = 1; i < n + 1; ++i) {
        int q = INT_MIN;
        for (int j = 1; j < i + 1; ++j) {
            q = max(q, price[j] + table[i - j]);
        }
        table[i] = q;
    }
    return table[n];
}

int extended_buttom_up_cut(int *price, int n)
{
    int *table = (int *) malloc(sizeof(int) * (n + 1));
    int *solution = (int *) malloc(sizeof(int) * (n + 1));
    table[0] = 0;
    for (int i = 1; i < n + 1; ++i) {
        int q = INT_MIN;
        for (int j = 1; j < i + 1; ++j) {
            if (q < price[j] + table[i - j]) {
                q = price[j] + table[i - j];
                solution[i] = j;
            }
        }
        table[i] = q;
    }
    int i = n;
    while (i > 0) {
        cout << solution[i] << " ";
        i = i - solution[i];
    }
    cout << endl;

    // print both table and solution
    cout << "r: ";
    for (int i = 0; i < n + 1; ++i) {
        cout << table[i] << ", ";
    }
    cout << "\n"
         << "s: ";
    for (int i = 0; i < n + 1; ++i) {
        cout << solution[i] << ", ";
    }
    cout << endl;
    return table[n];
}

int unmemoized_cut(int *price, int n)
{
    if (n == 0)
        return 0;
    int q = INT_MIN;
    for (int i = 1; i < n + 1; ++i) {
        q = max(q, price[i] + unmemoized_cut(price, n - i));
    }
    return q;
}

#define N 5

int main()
{
    // int price[N + 1] = {0, 1, 5, 8, 9, 10, 17, 17, 20};
    int price[N + 1] = {0, 2, 5, 6, 9, 10};
    int n = N;
    int max_price = extended_buttom_up_cut(price, n);
    // int max_price = unmemoized_cut(price, n);
    cout << "Max price: " << max_price << endl;
    return 0;
}