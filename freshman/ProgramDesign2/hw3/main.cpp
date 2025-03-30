#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
    fstream file(argv[1], ios::in);
    map<int, vector<string>> signIn;
    map<int, vector<string>> signOut;
    map<int, int> ID;
    map<int, int> overworkCount;
    map<int, int> forgetToSign;
    string line, cur, signType, time;

    while (getline(file, cur, ','), getline(file, signType, ','),
           getline(file, time, '\n')) {
        int id = stoi(cur);
        ID[id] = id;
        if (signType == "sign-in")
            signIn[id].push_back(time);
        else
            signOut[id].push_back(time);
    }

    for (auto it = ID.begin(); it != ID.end(); ++it) {
        int id = it->first;
        vector<string> signInTimes = signIn[id];
        vector<string> signOutTimes = signOut[id];
        int overwork = 0;
        int forget = 0;

        sort(signInTimes.begin(), signInTimes.end());
        sort(signOutTimes.begin(), signOutTimes.end());

        if (!signInTimes.size() || !signOutTimes.size()) {
            overworkCount[id] = 0;
            forgetToSign[id] = signInTimes.size() + signOutTimes.size();
            continue;
        }

        auto itOut = signOutTimes.begin(), itIn = signInTimes.begin();
        int date, date2;
        for (;;) {
            date = stoi((*itIn).substr(0, 8));
            date2 = stoi((*itOut).substr(0, 8));

            if (date < date2) {
                forget++;
                itIn++;
                if (itIn == signInTimes.end())
                    break;
                continue;
            }
            if (date > date2) {
                forget++;
                itOut++;
                if (itOut == signOutTimes.end())
                    break;
                continue;
            }
            int32_t minute_diff = stoi((*itOut).substr(8, 2)) * 60 +
                                  stoi((*itOut).substr(10, 2)) -
                                  stoi((*itIn).substr(8, 2)) * 60 -
                                  stoi((*itIn).substr(10, 2));
            if (minute_diff > 480) {
                ++overwork;
            }
            itIn++;
            itOut++;
            if (signOutTimes.end() == itOut)
                break;
            if (signInTimes.end() == itIn)
                break;
        }

        if (signOutTimes.end() == itOut)
            forget += signInTimes.end() - itIn;
        else
            forget += signOutTimes.end() - itOut;
        overworkCount[id] = overwork;
        forgetToSign[id] = forget;
    }

    for (auto it = overworkCount.begin(); it != overworkCount.end(); ++it) {
        int id = it->first;
        int overwork = it->second;
        int forget = forgetToSign[id];
        cout << id << "," << overwork << "," << forget << endl;
    }

    return 0;
}
