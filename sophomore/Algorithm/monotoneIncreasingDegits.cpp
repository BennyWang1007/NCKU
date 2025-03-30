
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

int monotoneIncreasingDegits(int num)
{
    char s[11];
    sprintf(s, "%d", num);
    int len = strlen(s);
    int flag = len;
    for (int i = len - 1; i > 0; --i) {
        if (s[i - 1] > s[i]) {
            s[i - 1]--;
            flag = i;
        }
    }
    for (int i = flag; i < len; ++i) {
        s[i] = '9';
    }
    return atoi(s);
}


int main()
{
    cout << monotoneIncreasingDegits(10) << "\n";
    cout << monotoneIncreasingDegits(1234) << "\n";
    cout << monotoneIncreasingDegits(332) << "\n";
    return 0;
}