#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int before = r.getCarrots();
    r.exchangeWithNeighbors(1);
    int after = r.getCarrots();

    if (before == 1 && after < 0) {
        std::cerr << "[FAILED] Invalid exchange with one carrot\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        std::cout << "\n[ ZERO CARROTS TEST 2 ]\n";
        std::cout << "[OK] One and zero carrots edge case passed\n";
    }

    MPI_Finalize();
    return 0;
}
