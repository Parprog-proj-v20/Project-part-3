#include <iostream>
#include <vector>
#include <cmath>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();
    r.distributeSpecialFood(300);
    r.exchangeWithNeighbors(10);

    int local_carrots = r.getCarrots();
    std::vector<int> all_carrots;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Gather(&local_carrots, 1, MPI_INT, all_carrots.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (r.getRank() == 0) {
        double sum = 0.0;
        for (int c : all_carrots) sum += c;
        double mean = sum / all_carrots.size();

        double variance = 0.0;
        for (int c : all_carrots) variance += (c - mean) * (c - mean);
        variance /= all_carrots.size();

        double stddev = std::sqrt(variance);

        std::cout << "[ VALIDATION TEST ]\n";
        std::cout << "Mean: " << mean << "\n";
        std::cout << "Variance: " << variance << "\n";
        std::cout << "StdDev: " << stddev << "\n";

        if (mean < 0 && variance < 0 && stddev < 0) {
            std::cerr << "[FAILED] Mean, variance and standard deviation are negative\n";
            return 1;
        }
        else std::cout << "[OK] Validation test passed\n";
    }

    MPI_Finalize();
    return 0;
}