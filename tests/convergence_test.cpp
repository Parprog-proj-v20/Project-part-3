#include <iostream>
#include <vector>
#include <cmath>
#include "Rabbits.h"

double calc_variance(const std::vector<int>& v) {
    double sum = 0;
    for (int x : v) sum += x;
    double mean = sum / v.size();

    double var = 0;
    for (int x : v)
        var += (x - mean) * (x - mean);

    return var / v.size();
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();
    r.distributeSpecialFood(300);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local = r.getCarrots();
    std::vector<int> all;
    if (rank == 0) all.resize(size);

    MPI_Gather(&local, 1, MPI_INT, rank == 0 ? all.data() : nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double var_before = 0;
    if (rank == 0) var_before = calc_variance(all);

    r.exchangeWithNeighbors(10);

    local = r.getCarrots();
    MPI_Gather(&local, 1, MPI_INT, rank == 0 ? all.data() : nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double var_after = calc_variance(all);

        std::cout << "\n[ CONVERGENCE TEST ]\n";
        if (var_after > var_before) {
            std::cerr << "[FAILED] Variance did not decrease\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        std::cout << "[OK] Convergence test passed\n";
        std::cout << "Variance before: " << var_before
            << "\nVariance after: " << var_after << "\n";
    }

    MPI_Finalize();
    return 0;
}
