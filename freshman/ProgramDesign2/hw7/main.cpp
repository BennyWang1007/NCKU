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

// #define TEST_TIME
// #define DEBUG
#define WITH_THREAD
#define THREAD_NUM 7

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
unordered_map<std::string, std::vector<pair<int, int>>>
    dictionary;  // dictionary[word] = vec{doc_id, word_count}
// unordered_map<std::string, std::vector<pair<int, double>>> dictionary_tf;
// //dictionary[word] = vec{doc_id, tf}
unordered_map<std::string, std::unordered_map<int, double>>
    dictionary_tf;  // dictionary[word][doc_id] = tf

unordered_map<string, double> word_idf;  // word_idf[word] = idf
// vector<int> indexes;
// unordered_map<int, int> doc_length;

// unordered_map<std::string, double> word_score;
string corpusName, queryName;
// ifstream query;
int32_t corpus_line_count = 0;
int32_t k;
std::mutex mtx;

#ifdef DEBUG

void print_dictionary(ofstream &out)
{
    out << "total line count : " << corpus_line_count << "\n";
    for (auto &word : dictionary) {
        out << word.first << " : ";
        for (auto &doc : word.second) {
            out << "{" << doc.first << ", " << doc.second << "}, ";
        }
        out << "\n";
    }
}

void print_dictionary_tf(ofstream &out)
{
    out << "total line count : " << corpus_line_count << "\n";
    for (auto &word : dictionary_tf) {
        out << word.first << " : ";
        for (auto &doc : word.second) {
            out << "{" << doc.first << ", " << doc.second << "}, ";
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
            cout << "{" << doc.first << ", " << doc.second << "}, ";
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
{  // transform line to lower case and remove all non-alphabet characters
    // int32_t length = line.length();
    string new_line(line.length() + 1, '\0');
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

inline void getWord(string &line, string &word, size_t &s_start)
{  // get the first word in line from s_start

    size_t i = 0;
    word.resize(line.length() - s_start);
    for (; s_start < line.length(); ++s_start) {
        if (96 < line[s_start] && line[s_start] < 123) {  // 97~122 : 'a'~'z'
            word[i] = line[s_start];
            ++i;
            continue;
        }
        if (64 < line[s_start] && line[s_start] < 91) {  // 65~90 : 'A'~'Z'
            word[i] = line[s_start] + 32;
            ++i;
            continue;
        }
        if (line[s_start] == ' ') {
            ++s_start;
            break;
        }
    }
    word.resize(i);
}

vector<string> corpus_lines[THREAD_NUM];

void readCorpus_thread_arg(int thread_id)
{
    size_t pos0;
    string word;
    size_t s_start;  //, s_end;
    int32_t word_count;
    unordered_map<string, vector<pair<int, int>>> dict_temp;
    unordered_map<string, unordered_map<int, double>> dict_tf_temp;
    unordered_map<string, int> occur_time;

    for (string &line : corpus_lines[thread_id]) {
        word_count = 0;
        occur_time.clear();
        pos0 = line.find_first_of(',');
        int32_t index = stoi(line.substr(0, pos0));

        s_start = 0;
        for (;;) {
            if (s_start >= line.length())
                break;
            getWord(line, word, s_start);
            if (word == "" or word == " ")
                continue;
            ++word_count;
            ++occur_time[word];
        }

        for (auto &p : occur_time) {
            dict_temp[p.first].emplace_back(
                index, p.second);  // dict[word] = vec{doc_id, occur_time}
            dict_tf_temp[p.first][index] =
                (double) p.second /
                word_count;  // dict_tf[word] = map{doc_id, tf}
        }
    }

    mtx.lock();
    for (auto &p : dict_temp) {
        for (auto &i : p.second) {
            dictionary[p.first].emplace_back(i);
        }
    }
    for (auto &p : dict_tf_temp) {
        for (auto &i : p.second) {
            dictionary_tf[p.first][i.first] = i.second;
        }
    }
    mtx.unlock();

    corpus_lines[thread_id].clear();
    corpus_lines[thread_id].shrink_to_fit();
}

void readCorpus_thread()
{
    ifstream corpus(corpusName, ios::in);
    int MAXCOUNT = THREAD_NUM * 100, cur_count = 0;

    string line;
    bool flag = false;
    for (;;) {
        for (int i = 0; i < THREAD_NUM; ++i) {
            if (getline(corpus, line)) {
                corpus_lines[i].emplace_back(line);
                ++corpus_line_count;
                ++cur_count;
                if (cur_count == MAXCOUNT) {
                    vector<thread> threads;
                    for (int i = 0; i < THREAD_NUM; ++i) {
                        threads.emplace_back(readCorpus_thread_arg, i);
                    }
                    for (auto &t : threads) {
                        t.join();
                    }
                    cur_count = 0;
                }
            } else {
                flag = true;
                break;
            }
        }
        if (flag)
            break;
    }

#ifdef TEST_TIME
    recordTime("corpusLine emplaced");
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
}

inline void calculate_idf()
{
    for (auto &p : dictionary) {
        word_idf[p.first] =
            log((double) corpus_line_count / (double) p.second.size());
    }
}

// inline void rm_dup_indexes(){
//     sort(indexes.begin(), indexes.end());
//     indexes.erase(unique(indexes.begin(), indexes.end()), indexes.end());
// }

// update score for a single word
inline void calculate_single(string &word, unordered_map<int, double> &score)
{
    double idf =
        log((double) corpus_line_count / (double) (dictionary[word]).size());
    for (auto &i : dictionary_tf[word]) {
        score[i.first] += idf * i.second;
    }
}

// return true if a < b
bool score_cmp(const pair<int, double> &a, const pair<int, double> &b)
{
    return b.second > a.second || (b.second == a.second && b.first < a.first);
}

void readCorpus()
{
    ifstream corpus(corpusName, ios::in);
    string line, word;
    size_t s_start;
    int index;
    while (getline(corpus, line)) {
        int word_count = 0;
        unordered_map<string, int> occurs_time;
        index = stoi(line.substr(0, line.find_first_of(",")));
        s_start = 0;
        while (s_start < line.length()) {
            getWord(line, word, s_start);
            if (word == "" || word == " ")
                continue;
            ++word_count;
            ++occurs_time[word];
        }
        for (auto &a : occurs_time) {
            dictionary[a.first].emplace_back(index, a.second);
            dictionary_tf[a.first][index] =
                (double) a.second / (double) word_count;
        }
    }
}

inline void print_topK(unordered_map<int, double> &score)
{
    vector<pair<int, double>> score_vec;
    score_vec.reserve(score.size() + 3);
    score_vec.emplace_back(-1, -1);
    score_vec.emplace_back(-1, -1);
    score_vec.emplace_back(-1, -1);
    for (auto a : score)
        score_vec.emplace_back(a);

    sort(score_vec.begin(), score_vec.end(),
         [](const pair<int, double> &a, const pair<int, double> &b) {
             return (a.second > b.second ||
                     (a.second == b.second && a.first < b.first));
         });
    for (int i = 0; i < k - 1; ++i) {
        cout << score_vec[i].first << " ";
    }
    cout << score_vec[k - 1].first << "\n";
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

vector<string> query_lines;
vector<int> results[THREAD_NUM];

void getMaxIdfsWords(vector<string> &line_words,
                     vector<string> &max_idf_words,
                     int index)
{
    max_idf_words.clear();
    vector<pair<string, double>> idf_words;
    for (auto &word : line_words) {
        for (auto &dict_pair : dictionary[word]) {
            if (dict_pair.first == index) {
                idf_words.emplace_back(word, word_idf[word]);
                break;
            }
        }
    }

    if (idf_words.size() < 3) {
        for (auto &word : idf_words) {
            max_idf_words.emplace_back(word.first);
        }
        return;
    }

    for (size_t i = 0; i < idf_words.size(); ++i) {
        for (size_t j = i + 1; j < idf_words.size(); ++j) {
            if (idf_words[i].second < idf_words[j].second ||
                (idf_words[i].second == idf_words[j].second &&
                 dictionary_tf[idf_words[i].first][index] <
                     dictionary_tf[idf_words[j].first][index])) {
                swap(idf_words[i], idf_words[j]);
            }
        }
    }

    for (size_t i = 0; i < 3; ++i) {
        max_idf_words.emplace_back(idf_words[i].first);
    }
    return;
}

// store top k score of start-th to endth line to results[thread_id]
void get_lines_result_thread(size_t start, size_t end, int32_t thread_id)
{
    results[thread_id].reserve((end - start) * k);
    string line, word;
    size_t s_start;

    for (size_t i = start; i < end; ++i) {
        line = query_lines[i];
        line_transform(line);
        unordered_map<int, double> score;
        vector<string> line_words;
        vector<string> max_word;
        vector<int> all_index;

        s_start = 0;

        for (;;) {
            if (s_start >= line.length())
                break;
            getWord(line, word, s_start);
            if (word == "" || word == " ")
                continue;
            line_words.emplace_back(word);
            for (auto &dict_pair : dictionary[word]) {
                all_index.emplace_back(dict_pair.first);
            }
        }
        // remove duplicate index
        sort(all_index.begin(), all_index.end());
        all_index.erase(unique(all_index.begin(), all_index.end()),
                        all_index.end());

        for (auto &index : all_index) {
            getMaxIdfsWords(line_words, max_word, index);
            for (auto &word : max_word) {
                if (word == "")
                    continue;
                score[index] += word_idf[word] * dictionary_tf[word][index];
            }
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

void readQuery_thread()
{
    ifstream query(queryName, ios::in);
    std::stringstream ss;
    string line, word;

    vector<thread> threads;

    while (getline(query, line)) {
        query_lines.emplace_back(line);
    }
    int line_num = query_lines.size();
    size_t line_per_thread = line_num / THREAD_NUM;
    size_t start_pos = 0, end_pos = line_per_thread;

    for (int i = 0; i < THREAD_NUM; ++i) {
        if (i != THREAD_NUM - 1) {
            threads.emplace_back(
                thread(get_lines_result_thread, start_pos, end_pos, i));
            start_pos = end_pos;
            end_pos += line_per_thread;
            continue;
        }
        threads.emplace_back(
            thread(get_lines_result_thread, start_pos, line_num, i));
        break;
    }
    for (auto &t : threads) {
        t.join();
    }

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
            continue;
        }
        if (k == 2) {
            for (;;) {
                if (j >= end)
                    break;
                cout << results[i][j] << " " << results[i][j + 1] << "\n";
                j += 2;
            }
            continue;
        }
        for (;;) {
            if (j >= end)
                break;
            cout << results[i][j] << "\n";
            ++j;
        }
    }
}


int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);

#ifdef TEST_TIME
    recordTime("start");
#endif

    corpusName = argv[1];
    queryName = argv[2];
    k = stoi(argv[3]);

#ifdef DEBUG
    ofstream score_file("score.txt", ios::out);
    score_file.close();
#endif

#ifdef WITH_THREAD
    readCorpus_thread();
    calculate_idf();
    // rm_dup_indexes();
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
    ofstream dict_tf_file("dict_tf.txt", ios::out);
    print_dictionary_tf(dict_tf_file);
    dict_tf_file.close();
#endif

#ifdef TEST_TIME
    recordTime("finish");
#endif

#ifdef TEST_TIME
    cerr << "time: " << (double) (clock() - start) / CLOCKS_PER_SEC << "\n";
#endif

    return 0;
}
