#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#define RECORD_TIME

#define plot_histogram(histogram, msg)                                 \
    std::cout << msg << std::endl;                                     \
    for (const auto &entry : histogram) {                              \
        std::cout << entry.first << ": " << entry.second << std::endl; \
    }                                                                  \
    std::cout << std::endl;


class RandomWalk
{
private:
    size_t test_count = 0;
    size_t dimension = 0;
    size_t step_count = 0;

    std::vector<std::vector<int>> position;  // position of each test
    std::vector<int> sections_count;         // count of each sections
    size_t prev_orgin_step = 0;              // previous origin step
    std::vector<int> origin_steps;           // steps between two origin steps
    size_t origin_step_count = 0;            // number of origin steps
    std::vector<int> n_plus_count;           // count of n+
    std::vector<int> n_minus_count;          // count of n-
    std::vector<int> n_zero_count;           // count of n0

    std::random_device rd;
    std::mt19937 gen{42};  // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis{0, 1};

public:
    RandomWalk(size_t test_count, size_t dimension, size_t step_count)
        : test_count(test_count), dimension(dimension), step_count(step_count)
    {
        position.resize(test_count, std::vector<int>(dimension, 0));
        sections_count.resize(1 << dimension, 0);
        if (dimension == 1) {
            n_plus_count.resize(test_count, 0);
            n_minus_count.resize(test_count, 0);
            n_zero_count.resize(test_count, 0);
        }
    }

    ~RandomWalk() {}

    void walk()
    {
        for (size_t i = 0; i < test_count; i++) {
            prev_orgin_step = 0;
            for (size_t j = 0; j < step_count; j++) {
                // for (size_t k = 0; k < dimension; k++) {
                //     position[i][k] += dis(gen) ? 1 : -1;
                // }

                // use bit operations to run faster
                int random_num = rand();
                if (dimension == 1) {
                    position[i][0] += (random_num & 1) ? 1 : -1;
                } else if (dimension == 2) {
                    position[i][0] += (random_num & 1) ? 1 : -1;
                    position[i][1] += (random_num & 2) ? 1 : -1;
                } else if (dimension == 3) {
                    position[i][0] += (random_num & 1) ? 1 : -1;
                    position[i][1] += (random_num & 2) ? 1 : -1;
                    position[i][2] += (random_num & 4) ? 1 : -1;
                } else if (dimension == 4) {
                    position[i][0] += (random_num & 1) ? 1 : -1;
                    position[i][1] += (random_num & 2) ? 1 : -1;
                    position[i][2] += (random_num & 4) ? 1 : -1;
                    position[i][3] += (random_num & 8) ? 1 : -1;
                }
                collect_data(position[i], j + 1, i);
            }
        }
    }

    void collect_data(std::vector<int> &pos, size_t step, size_t test_num)
    {
        bool has_zero = false;
        bool at_origin = true;

        // check if the position has at least one zero
        for (size_t i = 0; i < dimension; i++) {
            if (pos[i] == 0) {
                has_zero = true;
                break;
            }
        }

        if (has_zero) {
            // check if the position is at the origin
            for (size_t i = 0; i < dimension; i++) {
                if (pos[i] != 0) {
                    at_origin = false;
                    break;
                }
            }
        } else {
            at_origin = false;
            // record the section of the position
            size_t section_index = 0;
            for (size_t i = 0; i < dimension; i++) {
                if (pos[i] > 0) {
                    section_index |= 1 << i;
                }
            }
            sections_count[section_index]++;
        }

        // record the steps between two origin steps
        if (at_origin) {
            origin_steps.push_back(step - prev_orgin_step);
            prev_orgin_step = step;
            origin_step_count++;
        }

        // record n+, n-, n0
        if (dimension == 1) {
            if (pos[0] > 0) {
                n_plus_count[test_num]++;
            } else if (pos[0] < 0) {
                n_minus_count[test_num]++;
            } else {
                n_zero_count[test_num]++;
            }
        }
    }

    const std::vector<std::vector<int>> &getPosition() const
    {
        return position;
    }

    const std::vector<int> &getSectionsCount() const { return sections_count; }

    std::vector<int> getOriginSteps() { return origin_steps; }

    std::vector<int> getNPlusCount() { return n_plus_count; }
    std::vector<int> getNMinusCount() { return n_minus_count; }
    std::vector<int> getNZeroCount() { return n_zero_count; }

    size_t getOriginStepCount() { return origin_step_count; }
};


int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t dimensions[] = {1, 2, 3, 4};
    size_t steps[] = {100, 1000, 10000, 100000, 1000000};
    size_t walk_count = 1000;

#ifdef RECORD_TIME
    auto start = std::chrono::system_clock::now();
#endif

    for (auto &dim : dimensions) {
        for (auto &step : steps) {
            RandomWalk randomWalk(walk_count, dim, step);
            randomWalk.walk();

            std::string dim_step_str = "dim: " + std::to_string(dim) +
                                       ", step:" + std::to_string(step);
            std::cout << dim_step_str << std::endl;

            const auto &position = randomWalk.getPosition();
            // TODO: save the data to a file

            const auto &sections_count = randomWalk.getSectionsCount();
            // TODO: save the data to a file

            // plot histogram of sections count
            std::map<int, int> histogram_sections;
            for (size_t i = 0; i < sections_count.size(); i++) {
                histogram_sections[i] = sections_count[i];
            }
            std::string msg_sections =
                "Histogram of sections count (" + dim_step_str + ")";
            plot_histogram(histogram_sections, msg_sections);

            auto origin_steps = randomWalk.getOriginSteps();
            auto origin_step_count = randomWalk.getOriginStepCount();

            assert(origin_steps.size() == origin_step_count);

            std::sort(origin_steps.begin(), origin_steps.end());

            if (origin_step_count > 0) {
                long long sum = 0;
                for (size_t i = 0; i < origin_steps.size(); i++) {
                    sum += origin_steps[i];
                }
                std::cout << "Average: " << (double) sum / origin_steps.size()
                          << std::endl;

                // // plot histogram of origin steps
                // std::map<int, int> histogram_origin_steps;
                // for (const auto &step : origin_steps) {
                //     histogram_origin_steps[step]++;
                // }
                // std::string msg_origin_step =
                //     "Histogram of origin steps (" + dim_step_str + ")";
                // plot_histogram(histogram_origin_steps, msg_origin_step);

            } else {
                std::cout << "No origin steps" << std::endl << std::endl;
            }

            // experiment of m / n for dimension 1
            if (dim == 1) {
                std::vector<int> n_plus_count = randomWalk.getNPlusCount();
                std::vector<int> n_minus_count = randomWalk.getNMinusCount();
                std::vector<int> n_zero_count = randomWalk.getNZeroCount();
                size_t test_count = n_plus_count.size();

                std::map<int, int> histogram_m;
                std::map<double, int> histogram_m_devide_n;

                std::cout << std::endl;
                for (size_t i = 0; i < test_count; i++) {
                    double m = (double) n_zero_count[i] / 2 +
                               std::max(n_plus_count[i], n_minus_count[i]);
                    histogram_m[m]++;
                    histogram_m_devide_n[m / step]++;
                }

                // // plot histogram of m
                // std::string msg_m = "Histogram of m (" + dim_step_str + ")";
                // plot_histogram(histogram_m, msg_m);

                // // plot histogram of m / n
                // std::string msg_m_devide_n =
                //     "Histogram of m / n (" + dim_step_str + ")";
                // plot_histogram(histogram_m_devide_n, msg_m_devide_n);
            }
        }
    }

#ifdef RECORD_TIME
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << "s\n";
#endif

    return 0;
}
