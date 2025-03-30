#include <cstdlib>
#include <iostream>
#include <limits>

using namespace std;

void greedy_activity_selector(int *s, int *f, int n)
{
    int i = 0;
    cout << i << " ";
    for (int m = 1; m < n; ++m) {
        if (s[m] >= f[i]) {
            cout << m << " ";
            // cout << "(m, i): (" << m << ", " << i << "), (s[m], f[i]): (" <<
            // s[m] << ", " << f[i] << ")\n";
            i = m;
        }
    }
    cout << "\n";
}

int main()
{
    int s[] = {1, 2, 4, 1, 5, 8, 9, 11, 13};
    int f[] = {3, 5, 7, 8, 9, 10, 11, 14, 16};
    int n = sizeof(s) / sizeof(s[0]);
    greedy_activity_selector(s, f, n);
    return 0;
}