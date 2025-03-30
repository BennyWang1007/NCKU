#include <time.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    clock_t start, end;
    start = clock();
    fstream file("hw3_test4.csv", ios::in);
    map<int, vector<string>> signIn;
    map<int, int> overworkCount, forgetToSign;
    string line, cur, signType, time;

    while (getline(file, cur, ','), getline(file, signType, ','),
           getline(file, time, '\n')) {
        int id = stoi(cur);
        signIn[id].push_back(time);
    }
    end = clock();
    long double elapsed = (long double) (end - start) / CLOCKS_PER_SEC;
    cout << "\nTotal time = " << elapsed << "\n\n";

    FILE *fp = fopen("hw3_test4.csv", "r");
    // map <int, vector<pair<int, int>> > sign;
    map<int, vector<long long>> sign;
    map<int, vector<string>> sign;
    start = clock();
    int id, count = 0, hour, minute;
    long long date;
    // char cur_time[12];
    // char trash[20];

    while (fscanf(fp, "%d,%*[^,],%12lld\n", &id, &date) != EOF) {
        while (fscanf(fp, "%d,%*[^,],%12lld\n", &id, &date) != EOF) {
            sign[id].push_back(date);
            // sort(sign.begin(),sign.end());
        }
        // for(auto it = sign.be)
        // cout<<"count = "<<count<<"\n";
        fclose(fp);
        end = clock();
        elapsed = (long double) (end - start) / CLOCKS_PER_SEC;
        cout << "\nTotal time = " << elapsed << "\n\n";

        return 0;
    }
}
