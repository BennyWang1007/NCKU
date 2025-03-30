#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define TEST_TIME

using namespace std;

int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
#ifdef TEST_TIME
    clock_t start = clock();
#endif
    FILE *fp = fopen(argv[1], "r");
    // fstream file (argv[1], ios::in);
    map<int, vector<string>> signIn;
    map<int, int> overworkCount, forgetToSign;
    string line, cur, signType, time;
    int id;
    while (fscanf(fp, "%d,%*s,%s", &id, time)) {
        // while(getline(file, cur, ','), getline(file, signType, ','),
        // getline(file, time, '\n')){ int id = stoi(cur);
        signIn[id].push_back(time);
        cout << time << "\n";
    }
    for (auto iter = signIn.begin(); iter != signIn.end(); ++iter) {
        int id = iter->first;
        vector<string> signInTimes = signIn[id];
        int overwork = 0;
        int forget = 0;

        sort(signInTimes.begin(), signInTimes.end());
        if (signInTimes.size() == 1) {
            cout << id << ",0,1\n";
            overworkCount[id] = 0;
            forgetToSign[id] = 1;
            continue;
            ;
        }
        auto it = signInTimes.begin(), it2 = it + 1;
        int date = stoi((*it).substr(0, 8)), date2 = stoi((*it2).substr(0, 8));

        for (;;) {
            if (date != date2) {
                forget++;
                it++;
                it2++;
                if (it2 == signInTimes.end()) {
                    forget++;
                    break;
                }
                date = date2;
                date2 = stoi((*it2).substr(0, 8));
                continue;
            }
            int32_t minute_diff =
                stoi((*it2).substr(8, 2)) * 60 + stoi((*it2).substr(10, 2)) -
                stoi((*it).substr(8, 2)) * 60 - stoi((*it).substr(10, 2));
            if (minute_diff > 480)
                ++overwork;

            if (it == signInTimes.end() - 2)
                break;
            if (it == signInTimes.end() - 3) {
                forget++;
                break;
            }
            it += 2;
            it2 += 2;
            date = stoi((*it).substr(0, 8));
            date2 = stoi((*it2).substr(0, 8));
        }
        overworkCount[id] = overwork;
        forgetToSign[id] = forget;
        cout << id << "," << overwork << "," << forget << "\n";
    }

#ifdef TEST_TIME
    clock_t end = clock();
    long double elapsed = (long double) (end - start) / CLOCKS_PER_SEC;
    cout << "\nTotal time = " << elapsed << '\n';
#endif

    return 0;
}
