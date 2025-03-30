#include <math.h>
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

#include <mutex>
#include <thread>


#define THREAD_NUM 7

using namespace std;


unordered_map<std::string, std::unordered_set<int>> dictionary;
// unordered_map<std::string, std::vector<int>> dictionary;
unordered_map<std::string, double> word_score;
ifstream query;
int32_t corpus_line_count = 0;
int32_t k;
std::mutex mtx;

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
        ++corpus_line_count;
        pos0 = line.find(',');
        int32_t index = stoi(line.substr(0, pos0));
        string loc = line.substr(pos0 + 3, line.length() - pos0 - 4);
        line_transform(loc);
        std::stringstream ss(loc);
        while (ss >> word)
            dictionary[word].emplace(index);
    }
}

vector<string> corpus_lines[THREAD_NUM];
unordered_map<string, unordered_set<int>> dictionary_thread[THREAD_NUM];

void readCorpus_thread_arg(int thread_id)
{
    int32_t pos0;
    string line, word;
    for (auto &line : corpus_lines[thread_id]) {
        // ++corpus_line_count;
        pos0 = line.find(',');
        int32_t index = stoi(line.substr(0, pos0));
        string loc = line.substr(pos0 + 3, line.length() - pos0 - 4);
        line_transform(loc);
        std::stringstream ss(loc);
        while (ss >> word)
            dictionary_thread[thread_id][word].emplace(index);
    }
}

void combine_dictionary()
{
    for (int i = 0; i < THREAD_NUM; ++i) {
        for (auto &p : dictionary_thread[i]) {
            dictionary[p.first].insert(p.second.begin(), p.second.end());
        }
        dictionary_thread[i].clear();
    }
}

void readCorpus_thread(char *&filename)
{
    ifstream corpus(filename, ios::in);
    string line, word;
    bool flag = true;
    while (flag) {
        for (int i = 0; i < THREAD_NUM; ++i) {
            if (getline(corpus, line))
                corpus_lines[i].emplace_back(line);
            else {
                flag = false;
                break;
            }
        }
    }

    vector<thread> threads;
    for (int i = 0; i < THREAD_NUM; ++i)
        threads.emplace_back(readCorpus_thread_arg, i);  // create threads
    for (auto &t : threads)
        t.join();          // join threads
    combine_dictionary();  // combine dictionary
}

// update score for a single word
void calculate_single(string &word, unordered_map<int, double> &score)
{
    for (auto &i : dictionary[word])
        score[i] += word_score[word];
}

// return true if a < b
bool score_cmp(const pair<int, double> &a, const pair<int, double> &b)
{
    return b.second > a.second || (b.second == a.second && b.first < a.first);
}

void print_topK(unordered_map<int, double> &score)
{
    if (score.empty()) {
        if (k == 3) {
            cout << "-1 -1 -1\n";
            return;
        }
        if (k == 2) {
            cout << "-1 -1\n";
            return;
        }
        cout << "-1\n";
        return;
    }

    if (k == 1) {
        auto it = max_element(score.begin(), score.end(), score_cmp);
        cout << it->first << "\n";
        return;
    }
    if (k == 2) {
        int max1 = -1, max2 = -1;
        int max1_index = -1, max2_index = -1;
        for (auto &a : score) {
            if (a.second > max1 || (a.second == max1 && a.first < max1_index)) {
                max2 = max1;
                max2_index = max1_index;
                max1 = a.second;
                max1_index = a.first;
                continue;
            }
            if (a.second > max2 || (a.second == max2 && a.first < max2_index)) {
                max2 = a.second;
                max2_index = a.first;
            }
        }
        cout << max1_index << " " << max2_index << "\n";
    }
    if (k == 3) {
        double max1 = -1, max2 = -1, max3 = -1;
        int max1_index = -1, max2_index = -1, max3_index = -1;
        for (auto &a : score) {
            if (a.second > max1 || (a.second == max1 && a.first < max1_index)) {
                max3 = max2;
                max3_index = max2_index;
                max2 = max1;
                max2_index = max1_index;
                max1 = a.second;
                max1_index = a.first;
                continue;
            }
            if (a.second > max2 || (a.second == max2 && a.first < max2_index)) {
                max3 = max2;
                max3_index = max2_index;
                max2 = a.second;
                max2_index = a.first;
                continue;
            }
            if (a.second > max3 || (a.second == max3 && a.first < max3_index)) {
                max3 = a.second;
                max3_index = a.first;
            }
        }
        cout << max1_index << " " << max2_index << " " << max3_index << "\n";
    }
}

// get top k of index to result
void get_topK_thread(unordered_map<int, double> &score, vector<int> &result)
{
    if (score.empty()) {
        if (k == 1) {
            result.emplace_back(-1);
            return;
        }
        if (k == 2) {
            result.emplace_back(-1);
            result.emplace_back(-1);
            return;
        }
        if (k == 3) {
            result.emplace_back(-1);
            result.emplace_back(-1);
            result.emplace_back(-1);
            return;
        }
    }
    if (k == 1) {
        auto it = max_element(score.begin(), score.end(), score_cmp);
        result.emplace_back(it->first);
        return;
    }
    if (k == 2) {
        double max1 = -1, max2 = -1;
        int max1_index = -1, max2_index = -1;
        for (auto &a : score) {
            if (a.second > max1 || (a.second == max1 && a.first < max1_index)) {
                max2 = max1;
                max2_index = max1_index;
                max1 = a.second;
                max1_index = a.first;
                continue;
            }
            if (a.second > max2 || (a.second == max2 && a.first < max2_index)) {
                max2 = a.second;
                max2_index = a.first;
            }
        }
        result.emplace_back(max1_index);
        result.emplace_back(max2_index);
        return;
    }
    if (k == 3) {
        double max1 = -1, max2 = -1, max3 = -1;
        int max1_index = -1, max2_index = -1, max3_index = -1;
        for (auto &a : score) {
            if (a.second > max1 || (a.second == max1 && a.first < max1_index)) {
                max3 = max2;
                max3_index = max2_index;
                max2 = max1;
                max2_index = max1_index;
                max1 = a.second;
                max1_index = a.first;
                continue;
            }
            if (a.second > max2 || (a.second == max2 && a.first < max2_index)) {
                max3 = max2;
                max3_index = max2_index;
                max2 = a.second;
                max2_index = a.first;
                continue;
            }
            if (a.second > max3 || (a.second == max3 && a.first < max3_index)) {
                max3 = a.second;
                max3_index = a.first;
            }
        }
        result.emplace_back(max1_index);
        result.emplace_back(max2_index);
        result.emplace_back(max3_index);
        return;
    }
}

// calculate score of each line with words in line
void calculate_score(string line)
{
    stringstream ss(line);
    string word;
    unordered_map<int, double> score;

    while (ss >> word)
        for (auto &i : dictionary[word])
            score[i] += word_score[word];
    print_topK(score);
}

vector<string> query_lines;
vector<int> results[THREAD_NUM];

void remove_zeros(unordered_map<int, double> &score)
{
    for (auto it = score.begin(); it != score.end();) {
        if (it->second == 0.0)
            it = score.erase(it);
        else
            ++it;
    }
}

// store top k score of start-th to endth line to results[thread_id]
void get_lines_result_thread(int32_t start, int32_t end, int32_t thread_id)
{
    string line;
    for (int32_t i = start; i < end; ++i) {
        line = query_lines[i];
        line_transform(line);
        stringstream ss(line);
        string word;
        unordered_map<int, double> score;

        while (ss >> word)
            for (auto &i : dictionary[word])
                score[i] += word_score[word];
        remove_zeros(score);
        get_topK_thread(score, results[thread_id]);
    }
}

inline void readQuery()
{
    std::stringstream ss;
    string line, word;

    while (getline(query, line)) {
        calculate_score(line);
    }
    query.close();
}

void readQuery_thread()
{
    std::stringstream ss;
    string line, word;
    vector<thread> threads;

    while (getline(query, line))
        query_lines.emplace_back(line);

    int line_num = query_lines.size();
    int line_per_thread = line_num / THREAD_NUM;
    int start_pos = 0, end_pos = line_per_thread;

    // create threads and join
    for (int i = 0; i < THREAD_NUM; i++) {
        if (i == THREAD_NUM - 1) {
            threads.emplace_back(
                thread(get_lines_result_thread, start_pos, line_num, i));
            break;
        }
        threads.emplace_back(
            thread(get_lines_result_thread, start_pos, end_pos, i));
        start_pos = end_pos;
        end_pos += line_per_thread;
    }
    for (auto &t : threads)
        t.join();
    query.close();
    // print results
    for (int i = 0; i < THREAD_NUM; ++i) {
        // cout<<"\n-------------thread "<<i<<" ----------------\n";
        int end = results[i].size(), j = 0;
        if (k == 3) {
            for (;;) {
                if (j >= end)
                    break;
                cout << results[i][j] << " " << results[i][++j] << " "
                     << results[i][++j] << "\n";
                ++j;
            }
        } else if (k == 2) {
            for (;;) {
                if (j >= end)
                    break;
                cout << results[i][j] << " " << results[i][++j] << "\n";
                ++j;
            }
        } else {
            for (;;) {
                if (j >= end)
                    break;
                cout << results[i][j] << "\n";
                ++j;
            }
        }
    }
}

// calculate by Inverse Document Frequency
inline void calculate_word_score()
{
    for (auto &word : dictionary) {
        word_score[word.first] =
            log((double) corpus_line_count / (double) word.second.size());
    }
}

// calculate by Inverse Document Frequency
void calculate_word_score_thread_arg(int start_pos, int end_pos)
{
    unordered_map<string, unordered_set<int>>::iterator start =
                                                            dictionary.begin(),
                                                        end =
                                                            dictionary.begin();
    std::advance(start, start_pos);
    std::advance(end, end_pos);

    for (;;) {
        if (start == end)
            break;
        double idf =
            log((double) corpus_line_count / (double) (start->second).size());
        if (idf != 0.0) {
            unique_lock<mutex> lock(mtx);
            word_score[start->first] = idf;
            lock.unlock();
            ++start;
        }
    }
}

// create threads to calculate words' idf
void calculate_word_score_thread()
{
    int proccess_size = dictionary.size() / THREAD_NUM;
    std::vector<std::thread> threads;

    for (int i = 0;; ++i) {
        if (i == THREAD_NUM - 1) {
            threads.emplace_back(calculate_word_score_thread_arg,
                                 i * proccess_size, dictionary.size());
            break;
        }
        threads.emplace_back(calculate_word_score_thread_arg, i * proccess_size,
                             (i + 1) * proccess_size);
    }
    for (auto &t : threads)
        t.join();
}

int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    k = stoi(argv[3]);
    readCorpus(argv[1]);  // read corpus
    // readCorpus_thread(argv[1]);  //read corpus with thread
    calculate_word_score();
    // calculate_word_score_thread();

    query.open(argv[2]);

    // readQuery(); //read query and search
    readQuery_thread();  // read query and search

#ifdef TEST_TIME
    cout << "\nfinish\n";
    recordTime();
#endif

    return 0;
}
