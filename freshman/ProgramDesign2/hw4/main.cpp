#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// #define TEST_TIME

#define DIGIT_SHIFT 27
#define SHIFT_NUM 134217728

using namespace std;

bool cmp(pair<int, int> &a, pair<int, int> &b)
{
    return (a.second > b.second) || (a.second == b.second && a.first < b.first);
}

struct IntHash {
    size_t operator()(int x) const
    {
        return x % 1000000007; // choose a prime number close to 10^9
    }
};


#ifdef TEST_TIME
clock_t start = -1, end = 0;


void recordTime()
{
    if (start == -1)
        start = clock();
    else {
        long double elapsed = (long double) (clock() - start) / CLOCKS_PER_SEC;
        cout << "Total time = " << elapsed << "\n\n";
    }
}
#endif

bool are_dates_successive(int32_t date1, int32_t date2)
{
    if (date2 - date1 == 1)
        return true;
    if (date2 - date1 < 31)
        return false;

    int32_t day2 = date2 % 100;
    if (day2 != 1)
        return false;

    // rests are all 1st day of month and not the same month

    int32_t year1, year2, month1, month2, day1;
    year1 = date1 / 10000;
    month1 = (date1 % 10000) / 100;
    day1 = date1 % 100;
    year2 = date2 / 10000;
    month2 = (date2 % 10000) / 100;

    // different year
    if (year2 != year1) {
        if (year2 == year1 + 1 && month2 == 1 && month1 == 12 && day1 == 31)
            return true;
        return false;
    }

    // different month
    if (month2 != month1 + 1)
        return false;
    // cross month (1, 3, 5, 7, 8, 10, 12)
    if (day1 == 31)
        return true;
    // cross month (4, 6, 9, 11)
    if (day1 == 30 &&
        (month1 == 4 || month1 == 6 || month1 == 9 || month1 == 11))
        return true;
    // cross leap year of feb
    if (day1 == 29 && month1 == 2 &&
        (year1 % 4 == 0 && (year1 % 100 != 0 || year1 % 400 == 0)))
        return true;
    // cross non-leap year of feb
    if (day1 == 28 && month1 == 2 &&
        (year1 % 4 != 0 || (year1 % 100 == 0 && year1 % 400 != 0)))
        return true;

    return false;
}

unordered_map<int, std::vector<int>, IntHash> signIn;
unordered_map<int, int, IntHash> startDate, endDate;

int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);

#ifdef TEST_TIME
    cout << "\nstart\n";
    recordTime();
#endif

    ifstream file(argv[1], ios::in);
    string line;

    while (getline(file, line)) {
        signIn[stoi(line.substr(0, line.find(',')))].push_back(
            stoi(line.substr(line.length() - 12, 8)));
    }

#ifdef TEST_TIME
    cout << "\nread file\n";
    recordTime();
#endif

    int32_t maxStart1 = 0, maxStart2 = 0, maxStart3 = 0;
    int32_t maxEnd1 = 0, maxEnd2 = 0, maxEnd3 = 0;
    int32_t maxSuccessiveDay1 = 0, maxSuccessiveDay2 = 0, maxSuccessiveDay3 = 0;
    int32_t maxId1 = -1, maxId2 = -1, maxId3 = -1;

    for (auto iter = signIn.begin(); iter != signIn.end(); ++iter) {
        int id = iter->first;
        vector<int> *signInTimes = &signIn[id];

        sort(signInTimes->begin(), signInTimes->end());

        int cur = *(signInTimes->begin());
        int lastDate = -1, successiveDay = 1, lastStart = cur;
        int maxSuccessiveDay = 1, maxStart = cur, maxEnd = cur;

        for (auto it = signInTimes->begin(); it != signInTimes->end(); ++it) {
            if (*it == lastDate)
                continue;
            if (are_dates_successive(lastDate, *it)) {
                successiveDay++;
                if (successiveDay >= maxSuccessiveDay) {
                    maxSuccessiveDay = successiveDay;
                    maxStart = lastStart;
                    maxEnd = *it;
                }
            } else {
                lastStart = *it;
                successiveDay = 1;
            }

            lastDate = *it;
        }

        if ((maxSuccessiveDay > maxSuccessiveDay1) ||
            (maxSuccessiveDay == maxSuccessiveDay1 && id < maxId1)) {
            maxSuccessiveDay3 = maxSuccessiveDay2;
            maxSuccessiveDay2 = maxSuccessiveDay1;
            maxSuccessiveDay1 = maxSuccessiveDay;
            maxStart3 = maxStart2;
            maxStart2 = maxStart1;
            maxStart1 = maxStart;
            maxEnd3 = maxEnd2;
            maxEnd2 = maxEnd1;
            maxEnd1 = maxEnd;
            maxId3 = maxId2;
            maxId2 = maxId1;
            maxId1 = id;
        } else if ((maxSuccessiveDay > maxSuccessiveDay2) ||
                   (maxSuccessiveDay == maxSuccessiveDay2 && id < maxId2)) {
            maxSuccessiveDay3 = maxSuccessiveDay2;
            maxSuccessiveDay2 = maxSuccessiveDay;
            maxStart3 = maxStart2;
            maxStart2 = maxStart;
            maxEnd3 = maxEnd2;
            maxEnd2 = maxEnd;
            maxId3 = maxId2;
            maxId2 = id;
        } else if ((maxSuccessiveDay > maxSuccessiveDay3) ||
                   (maxSuccessiveDay == maxSuccessiveDay3 && id < maxId3)) {
            maxSuccessiveDay3 = maxSuccessiveDay;
            maxStart3 = maxStart;
            maxEnd3 = maxEnd;
            maxId3 = id;
        }
    }

    // print top 3
    cout << maxId1 << "," << maxSuccessiveDay1 << "," << maxStart1 << ","
         << maxEnd1 << "\n";
    cout << maxId2 << "," << maxSuccessiveDay2 << "," << maxStart2 << ","
         << maxEnd2 << "\n";
    cout << maxId3 << "," << maxSuccessiveDay3 << "," << maxStart3 << ","
         << maxEnd3 << "\n";

#ifdef TEST_TIME
    cout << "\nfinish\n";
    recordTime();
#endif

    return 0;
}
