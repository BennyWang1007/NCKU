#include <iostream>

using namespace std;

int bagOfTokenScore(int *tokens, int len, int power)
{
    int score = 0;
    int p1 = 0, p2 = len - 1;
    while (p1 <= p2) {
        if (power >= tokens[p1]) {
            power -= tokens[p1];
            score++;
            p1++;
        } else if (score > 0 && p1 < p2) {
            power += tokens[p2];
            score--;
            p2--;
        } else {
            break;
        }
    }
    return score;
}

int main()
{
    int tokens1[] = {100};
    int power1 = 50;
    int tokens2[] = {100, 200};
    int power2 = 150;
    int tokens3[] = {100, 200, 300, 400};
    int power3 = 200;

    cout << bagOfTokenScore(tokens1, sizeof(tokens1) / sizeof(tokens1[0]),
                            power1)
         << "\n";
    cout << bagOfTokenScore(tokens2, sizeof(tokens2) / sizeof(tokens2[0]),
                            power2)
         << "\n";
    cout << bagOfTokenScore(tokens3, sizeof(tokens3) / sizeof(tokens3[0]),
                            power3)
         << "\n";
    return 0;
}
