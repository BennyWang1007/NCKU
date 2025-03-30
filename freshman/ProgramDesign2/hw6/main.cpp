#include <math.h>
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

#define TEST_TIME
// #define DEBUG
#define WITH_THREAD
#define THREAD_NUM 6

using namespace std;


#ifdef TEST_TIME

#include <time.h>
clock_t start = -1, end = 0;
ofstream out("time.txt", ios::out);


void recordTime(string s)
{
    ofstream out("time.txt", ios::app);
    if (start == -1)
        start = clock();
    else {
        long double elapsed = (long double) (clock() - start) / CLOCKS_PER_SEC;
        out << "\n" << s;
        out << "\nTotal time = " << elapsed << "\n\n";
    }
}
#endif

// unordered_map<std::string, std::unordered_set<int>> dictionary;
unordered_map<std::string, std::vector<int>> dictionary;
unordered_map<std::string, double> word_score;
string corpusName, queryName;
ifstream query;
int32_t corpus_line_count = 0;
int32_t k;
std::mutex mtx;

#ifdef DEBUG
void print_word_score(ofstream &out)
{
    for (auto &word : word_score) {
        out << word.first << " : " << word.second << "\n";
    }
}

void print_dictionary(ofstream &out)
{
    out << "total line count : " << corpus_line_count << "\n";
    for (auto &word : dictionary) {
        out << word.first << " : ";
        for (auto &doc : word.second) {
            out << doc << ", ";
        }
        out << "\n";
    }
}

void print_dictionary()
{
    cout << "total line count : " << corpus_line_count << "\n";
    for (auto &word : dictionary) {
        cout << word.first << " : ";
        for (auto &doc : word.second) {
            cout << doc << ", ";
        }
        cout << "\n";
    }
}

void print_all_score(unordered_map<int, double> &score)
{
    for (auto &i : score) {
        cout << i.first << " " << i.second << "\n";
    }
}
#endif

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
        // cout<<index<<" : "<<loc<<"\n";
        line_transform(loc);
        std::stringstream ss(loc);
        while (ss >> word) {
            // ++total_word_count;
            dictionary[word].emplace_back(index);
        }
        // print_dictionary();
    }
    // cout<<"total line count : "<<corpus_line_count<<"\n";
}

vector<string> corpus_lines[THREAD_NUM];
// unordered_map<string, unordered_set<int>> dictionary_thread[THREAD_NUM];
unordered_map<string, vector<int>> dictionary_thread[THREAD_NUM];

void readCorpus_thread_arg(int thread_id)
{
    size_t pos0;
    string word;
    size_t s_start, s_end;
    for (string &line : corpus_lines[thread_id]) {
        // ++corpus_line_count;
        pos0 = line.find_first_of(',');
        int32_t index = stoi(line.substr(0, pos0));
        line = line.substr(pos0 + 3, line.length() - pos0 - 4);
        line_transform(line);

        s_start = 0;
        s_end = line.find_first_of(' ');
        for (;;) {
            dictionary_thread[thread_id][line.substr(s_start, s_end - s_start)]
                .emplace_back(index);
            if (s_end == std::string::npos)
                break;

            s_start = s_end + 1;
            s_end = line.find_first_of(' ', s_start);
        }
    }
}

void combine_dictionary()
{
    for (int i = 0; i < THREAD_NUM; ++i) {
        for (auto &p : dictionary_thread[i]) {
            for (auto &j : p.second) {
                dictionary[p.first].emplace_back(j);
            }
        }
        dictionary_thread[i].clear();
    }
}

void readCorpus_thread_arg2(int thread_id)
{
    ifstream corpus(corpusName, ios::in);
    string line, id_str;
    size_t s_start, s_end;
    int index;
    for (int i = 0; i < thread_id; ++i) {
        corpus.ignore(1000000000, '\n');
        if (thread_id == 0)
            ++corpus_line_count;
    }

    for (;;) {
        if (!getline(corpus, id_str, ','))
            break;

        getline(corpus, line);

        line = line.substr(1, line.length() - 2);
        index = stoi(id_str);
        line_transform(line);
        s_start = 0;
        s_end = line.find_first_of(' ');
        for (;;) {
            dictionary_thread[thread_id][line.substr(s_start, s_end - s_start)]
                .emplace_back(index);
            if (s_end == std::string::npos)
                break;

            s_start = s_end + 1;
            s_end = line.find_first_of(' ', s_start);
        }

        if (thread_id == 0)
            ++corpus_line_count;
        for (int i = 0; i < THREAD_NUM - 1; ++i) {
            corpus.ignore(1000000000, '\n');
            if (corpus.eof())
                break;
            if (thread_id == 0)
                ++corpus_line_count;
        }
    }
}

void readCorpus_thread2()
{
    vector<thread> threads;
    for (int i = 0; i < THREAD_NUM; ++i) {
        threads.emplace_back(readCorpus_thread_arg2, i);
    }
    for (auto &t : threads) {
        t.join();
    }
#ifdef TEST_TIME
    recordTime("line emplaced");
#endif
    combine_dictionary();
}

void readCorpus_thread()
{
    ifstream corpus(corpusName, ios::in);
    string line;
    bool flag = false;
    for (;;) {
        for (int i = 0; i < THREAD_NUM; ++i) {
            if (getline(corpus, line)) {
                corpus_lines[i].emplace_back(line);
                ++corpus_line_count;
            } else {
                flag = true;
                break;
            }
        }
        if (flag)
            break;
    }
#ifdef TEST_TIME
    recordTime("line emplaced");
#endif


    vector<thread> threads;
    for (int i = 0; i < THREAD_NUM; ++i) {
        threads.emplace_back(readCorpus_thread_arg, i);
    }
    for (auto &t : threads) {
        t.join();
    }
#ifdef TEST_TIME
    recordTime("dictionary emplaced");
#endif
    combine_dictionary();
#ifdef TEST_TIME
    recordTime("dictionary combined");
#endif
}

// update score for a single word
inline void calculate_single(string &word, unordered_map<int, double> &score)
{
    for (auto &i : dictionary[word]) {
        score[i] += word_score[word];
    }
}

// return true if a < b
bool score_cmp(const pair<int, double> &a, const pair<int, double> &b)
{
    return b.second > a.second || (b.second == a.second && b.first < a.first);
}

inline void print_topK(unordered_map<int, double> &score)
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

// get top k of index to result, use loop unrolling to speed up
inline void get_topK_thread(unordered_map<int, double> &score,
                            vector<int> &result)
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
    if (k == 1) {
        auto it = max_element(score.begin(), score.end(), score_cmp);
        result.emplace_back(it->first);
        return;
    }
}

inline void remove_zeros(unordered_map<int, double> &score)
{
    for (auto it = score.begin(); it != score.end();) {
        if (it->second == 0.0)
            it = score.erase(it);
        else
            ++it;
    }
}

// calculate score of each line with words in line
inline void calculate_score(string line)
{
    line_transform(line);
    stringstream ss(line);
    string word;
    unordered_map<int, double> score;

    while (ss >> word) {
        calculate_single(word, score);
    }
    remove_zeros(score);
#ifdef DEBUG
    ofstream out("score.txt", ios::app);
    out << "\nscore of " << line << ":\n";
    for (auto &a : score) {
        out << a.first << ":" << a.second << "\n";
    }
    out.close();
#endif

    print_topK(score);
    // sort by second decending, first ascending
}

vector<string> query_lines;
vector<int> results[THREAD_NUM];

// store top k score of start-th to endth line to results[thread_id]
void get_lines_result_thread(int32_t start, int32_t end, int32_t thread_id)
{
    string line, word;
    size_t s_start, s_end;

    for (int32_t i = start; i < end; ++i) {
        line = query_lines[i];
        line_transform(line);
        unordered_map<int, double> score;

        s_start = 0;
        s_end = line.find_first_of(' ');

        for (;;) {
            // while(s_end != std::string::npos){
            word = line.substr(s_start, s_end - s_start);
            calculate_single(word, score);
            if (s_end == std::string::npos)
                break;
            s_start = s_end + 1;
            s_end = line.find_first_of(' ', s_start);
        }

        remove_zeros(score);
#ifdef DEBUG
        mtx.lock();
        ofstream out("score.txt", ios::app);
        out << "\nscore of " << line << ":\n";
        vector<pair<int, double>> score_map;
        for (auto &a : score) {
            score_map.emplace_back(a);
        }
        sort(score_map.begin(), score_map.end(),
             [](const pair<int, double> &a, const pair<int, double> &b) {
                 if (a.second == b.second)
                     return a.first < b.first;
                 return a.second > b.second;
             });
        for (auto &a : score_map) {
            out << a.first << ":" << a.second << "\n";
        }
        out.close();
        mtx.unlock();
#endif
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

    while (getline(query, line)) {
        query_lines.emplace_back(line);
    }
    int line_num = query_lines.size();
    int line_per_thread = line_num / THREAD_NUM;
    int start_pos = 0, end_pos = line_per_thread;

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
    for (auto &t : threads) {
        t.join();
    }
    query.close();

    // cout << "query finished\n";

#ifdef DEBUG
    ofstream out("results.txt", ios::out);
    for (int i = 0; i < THREAD_NUM; ++i) {
        out << "\n-------------thread " << i << " ----------------\n";
        for (auto &a : results[i]) {
            out << a << " ";
        }
    }
    out.close();
#endif
    int end, j;
    for (int i = 0; i < THREAD_NUM; ++i) {
        // cout<<"\n-------------thread "<<i<<" ----------------\n";
        end = results[i].size();
        j = 0;
        if (k == 3) {
            for (;;) {
                if (j >= end)
                    break;
                cout << results[i][j] << " " << results[i][j + 1] << " "
                     << results[i][j + 2] << "\n";
                j += 3;
            }
        } else if (k == 2) {
            for (;;) {
                if (j >= end)
                    break;
                cout << results[i][j] << " " << results[i][j + 1] << "\n";
                j += 2;
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
void calculate_word_score()
{
    for (auto &word : dictionary) {
        word_score[word.first] =
            log((double) corpus_line_count / (double) word.second.size());
    }
}



void calculate_word_score_thread_arg(int start_pos, int end_pos)
{
    // calculate by Inverse Document Frequency

    // unordered_map<string, unordered_set<int>>::iterator start =
    // dictionary.begin(), end = dictionary.begin();

    unordered_map<string, vector<int>>::iterator start = dictionary.begin(),
                                                 end = dictionary.begin();
    std::advance(start, start_pos);
    std::advance(end, end_pos);

    for (;;) {
        if (start == end)
            break;
        double idf =
            log((double) corpus_line_count / (double) (start->second).size());

        if (idf != 0.0) {
            mtx.lock();
            word_score[start->first] = idf;
            mtx.unlock();
            ++start;
        } else
            ++start;
    }
}

// calculate idf of each word via multi-threading
void calculate_word_score_thread()
{
    int thread_size = dictionary.size() / THREAD_NUM;
    vector<thread> threads;

    for (int i = 0; i < THREAD_NUM; ++i) {
        if (i == THREAD_NUM - 1) {
            threads.emplace_back(calculate_word_score_thread_arg,
                                 i * thread_size, dictionary.size());
            break;
        }
        threads.emplace_back(calculate_word_score_thread_arg, i * thread_size,
                             (i + 1) * thread_size);
    }
    for (auto &t : threads) {
        t.join();
    }
}



int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);

#ifdef TEST_TIME
    recordTime("start");
#endif

    k = stoi(argv[3]);
    query.open(argv[2]);
    corpusName = argv[1];
    queryName = argv[2];

#ifdef DEBUG
    ofstream score_file("score.txt", ios::out);
    score_file.close();
#endif

#ifdef WITH_THREAD
    readCorpus_thread();
    calculate_word_score_thread();
    readQuery_thread();
#else
    readCorpus(argv[1]);  // read corpus
    calculate_word_score();
    readQuery();  // read query and search
#endif

#ifdef DEBUG

    ofstream idf_file("idf.txt", ios::out);
    print_word_score(idf_file);
    idf_file.close();
    ofstream dict_file("dict.txt", ios::out);
    print_dictionary(dict_file);
    dict_file.close();
#endif

#ifdef TEST_TIME
    recordTime("finish");
#endif

    return 0;
}
