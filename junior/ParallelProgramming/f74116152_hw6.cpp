#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

#include "mpi.h"

using std::cin, std::cout, std::endl, std::string, std::ifstream, std::vector;

constexpr double alpha = 1.0;
constexpr double beta = 2.0;
constexpr double rho = 0.7;
constexpr double Q = 100;

constexpr int thread_num = 4;
int rank = 0, num_procs = 0;

template <typename T>
T** allocate_2d(int n, int m, T value = T()) {
    T** arr = new T*[n];
    for (int i = 0; i < n; i++) {
        arr[i] = new T[m];
        std::fill(arr[i], arr[i] + m, value);
    }
    return arr;
}

void readData(int &n, int &iter_num, int &m, int **&edges, string filename) {
    ifstream file(filename);
    file >> n >> m >> iter_num;
    edges = allocate_2d<int>(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file >> edges[i][j];
        }
    }
    file.close();
}

static inline void get_weights_and_sum(bool *visited, double **&pheromone, double **&rev_edges, int &i, const int &n, double *all_weight, double &weight_sum) {
    weight_sum = 0;
    for (int j = 0; j < n; j++) {
        if (visited[j] || i == j) {
            all_weight[j] = 0;
            continue;
        }
        all_weight[j] = pow(pheromone[i][j], alpha) * pow(rev_edges[i][j], beta);
        weight_sum += all_weight[j];
    }
}

static inline int select_city(bool *visited, int &current_city, double *all_weight, double &sum, const int &n) {
    double threshold = ((double)rand() / RAND_MAX) * sum;
    double cumulative = 0;
    for (int i = 0; i < n; i++) {
        if (visited[i]) continue;
        cumulative += all_weight[i];
        if (cumulative >= threshold) {
            return i;
        }
    }
    return -1;
}


void ant_algorithm_1iter(double **&pheromone, double **&delta_pheromone, const int &n, const int &m, int **&edges, double **&rev_edges, int &thread_min_cost) {
    // initialize delta_pheromone
#pragma omp parallel for num_threads(thread_num)
    for (int i = 0; i < n; i++) {
        std::fill(delta_pheromone[i], delta_pheromone[i] + n, 0.0);
    }
    int path[n+1];
    double weights[n];
    bool visited[n];
    // simulate ants
#pragma omp parallel for num_threads(thread_num) reduction(min:thread_min_cost) private(path, weights, visited)
    for (int ant = 0; ant < m; ++ant) {
        int current_city = rand() % n;
        int cost = 0;
        std::fill(visited, visited + n, false);
        visited[current_city] = true;
        path[0] = current_city;
        double weight_sum;
        for (int i = 0; i < n - 1; ++i) {
            get_weights_and_sum(visited, pheromone, rev_edges, current_city, n, weights, weight_sum);
            // randomly select next city, use weights as weight
            int next_city = select_city(visited, current_city, weights, weight_sum, n);
            visited[next_city] = true;
            cost += edges[current_city][next_city];
            path[i + 1] = next_city;
            current_city = next_city;
        }
        cost += edges[current_city][path[0]];
        path[n] = path[0];  // return to the start city

        if (cost < thread_min_cost) {
            thread_min_cost = cost;
        }
        // update delta_pheromone
        double delta_pheromone_ij = Q / cost;
#pragma omp critical
        {
            for (int idx = 0; idx < n; idx++) {
                delta_pheromone[path[idx]][path[idx+1]] += delta_pheromone_ij;
            }
        }
    }
    // update pheromone
#pragma omp parallel for num_threads(thread_num)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            pheromone[i][j] = (1 - rho) * pheromone[i][j] + delta_pheromone[i][j];
        }
    }
}


void ant_algorithm(const int n, const int iter_num, const int ant_num, int **edges, double **rev_edges, int &min_cost) {
    double **pheromone = allocate_2d<double>(n, n, 1.0);
    double **delta_pheromone = allocate_2d<double>(n, n);

    for (int iter = 0; iter < iter_num; iter++) {
        int iter_min_cost = std::numeric_limits<int>::max();
        ant_algorithm_1iter(pheromone, delta_pheromone, n, ant_num, edges, rev_edges, iter_min_cost);
        if (iter_min_cost < min_cost) {
            min_cost = iter_min_cost;
        }
        if (iter == iter_num - 1) break;
        // get the rank with the minimum cost
        int cost_rank_value = iter_min_cost * num_procs + rank; // making sure be able to identify the rank with same cost
        MPI_Allreduce(MPI_IN_PLACE, &cost_rank_value, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        int min_cost_rank = cost_rank_value % num_procs;
        // synchronize pheromone
        for (int i = 0; i < n; i++) {
            MPI_Bcast(pheromone[i], n, MPI_DOUBLE, min_cost_rank, MPI_COMM_WORLD);
        }
    }
    for (int i = 0; i < n; i++) {
        delete[] delta_pheromone[i];
        delete[] pheromone[i];
    }
    delete[] delta_pheromone;
    delete[] pheromone;
}




int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false); std::cin.tie(0);

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    std::srand(time(nullptr));

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int n, iter_num, m;
    int **edges;

    if (rank == 0) readData(n, iter_num, m, edges, argv[1]);

    // broadcast data
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&iter_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        edges = allocate_2d<int>(n, n);
    }
    for (int i = 0; i < n; i++) {
        MPI_Bcast(edges[i], n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    // calculate 1 / edges
    double **rev_edges = allocate_2d<double>(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            rev_edges[i][j] = 1.0 / edges[i][j];
        }
    }

    int min_cost = std::numeric_limits<int>::max();
    ant_algorithm(n, iter_num, m, edges, rev_edges, min_cost);

    // get the global minimum cost
    MPI_Allreduce(MPI_IN_PLACE, &min_cost, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    // print result
    if (rank == 0) {
        cout << min_cost << endl;
    }

    for (int i = 0; i < n; i++) {
        delete[] edges[i];
        delete[] rev_edges[i];
    }
    delete[] edges;
    delete[] rev_edges;

    MPI_Finalize();

    return 0;
}