#include <time.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


// #define TEST_TIME

#define SEARCH_THRESHOLD 250

using namespace std;


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

unordered_map<std::string, std::set<int>> dictionary;
ifstream query;

inline void get_intersection3(set<int> &result, const set<int> &target)
{
    if (result.empty())
        return;
    if (target.empty()) {
        result.clear();
        return;
    }

    if (result.size() < SEARCH_THRESHOLD) {
        std::set<int>::iterator it1 = result.begin();
        int max = *result.rbegin();
        while (it1 != result.end()) {
            if (*it1 > max)
                break;
            if (target.find(*it1) == target.end()) {
                result.erase(it1++);
                continue;
            }
            ++it1;
        }
        if (it1 != result.end())
            result.erase(it1, result.end());
        return;
    }

    if (target.size() < SEARCH_THRESHOLD) {
        std::set<int>::iterator it2 = target.begin();
        set<int> temp;
        int max = *result.rbegin();
        while (it2 != target.end()) {
            if (*it2 > max)
                break;
            if (result.find(*it2) != result.end()) {
                // temp.insert(*it2);
                temp.emplace_hint(temp.end(), *it2);
                ++it2;
                continue;
            }
            ++it2;
        }
        result = temp;
        return;
    }

    std::set<int>::iterator it1 = result.begin(), it2 = target.begin();
    while (it1 != result.end() && it2 != target.end()) {
        if (*it1 < *it2) {
            result.erase(it1++);
            continue;
        }
        if (*it1 > *it2) {
            it2++;
            continue;
        }
        it1++;
        it2++;
    }
    if (it1 != result.end())
        result.erase(it1, result.end());

    // result.erase(it1, result.end());
}



inline void print_result(set<int> &result)
{
    if (!result.empty()) {
        bool not_first = false;
        for (auto &index : result) {
            if (not_first)
                cout << " " << index;
            else {
                cout << index;
                not_first = true;
            }
        }
        cout << "\n";
    } else
        cout << "-1\n";
}

inline void line_transform(string &line)
{
    int32_t length = line.length();
    string new_line(length + 1, '\0');
    int i = 0;
    for (char &c : line) {
        if ((96 < c && c < 123) || c == ' ') {  // 97~122 : 'a'~'z'
            new_line[i] = c;
            ++i;
            continue;
        }
        if (64 < c && c < 91) {  // 65~90 : 'A'~'Z'
            new_line[i] = c + 32;
            ++i;
            continue;
        }
    }
    new_line.resize(i);
    line = new_line;
}

inline void readCorpus(char *&filename)
{
    ifstream corpus(filename, ios::in);
    int32_t pos0;
    string line, word;
    while (getline(corpus, line)) {
        pos0 = line.find(',');

        int32_t index = stoi(line.substr(0, pos0));
        string loc = line.substr(pos0 + 3, line.length() - pos0 - 4);
        line_transform(loc);
        std::stringstream ss(loc);
        while (ss >> word) {
            dictionary[word].emplace(index);
        }
    }
}



inline void readQuery()
{
    // ifstream query(filename, ios::in);
    std::stringstream ss;
    string line, word;

    while (getline(query, line)) {
        line_transform(line);
        ss = stringstream(line);

        vector<string> words;
        while (ss >> word)
            words.push_back(word);
        if (words.size() == 1) {
            set<int> result = dictionary[words[0]];
            print_result(result);
            continue;
        }
        if (words.size() == 2) {
            set<int> result = dictionary[words[0]];
            get_intersection3(result, dictionary[words[1]]);
            print_result(result);
            continue;
        }
        if (words.size() == 3) {
            int size1 = dictionary[words[0]].size(),
                size2 = dictionary[words[1]].size(),
                size3 = dictionary[words[2]].size();
            if (size1 >= size2 && size2 >= size3) {
                set<int> result = dictionary[words[2]];
                get_intersection3(result, dictionary[words[1]]);
                if (!result.empty())
                    get_intersection3(result, dictionary[words[0]]);
                print_result(result);
                continue;
            } else if (size1 >= size3 && size3 >= size2) {
                set<int> result = dictionary[words[1]];
                get_intersection3(result, dictionary[words[2]]);
                if (!result.empty())
                    get_intersection3(result, dictionary[words[0]]);
                print_result(result);
                continue;
            } else if (size2 >= size1 && size1 >= size3) {
                set<int> result = dictionary[words[2]];
                get_intersection3(result, dictionary[words[0]]);
                if (!result.empty())
                    get_intersection3(result, dictionary[words[1]]);
                print_result(result);
                continue;
            } else if (size2 >= size3 && size3 >= size1) {
                set<int> result = dictionary[words[0]];
                get_intersection3(result, dictionary[words[2]]);
                if (!result.empty())
                    get_intersection3(result, dictionary[words[1]]);
                print_result(result);
                continue;
            } else if (size3 >= size1 && size1 >= size2) {
                set<int> result = dictionary[words[1]];
                get_intersection3(result, dictionary[words[0]]);
                if (!result.empty())
                    get_intersection3(result, dictionary[words[2]]);
                print_result(result);
                continue;
            } else if (size3 >= size2 && size2 >= size1) {
                set<int> result = dictionary[words[0]];
                get_intersection3(result, dictionary[words[1]]);
                if (!result.empty())
                    get_intersection3(result, dictionary[words[2]]);
                print_result(result);
                continue;
            }
        }
    }
}

int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    readCorpus(argv[1]);  // read corpus

    query.open(argv[2]);

    readQuery();  // read query and search

    return 0;
}
