#include <iostream>
#include <vector>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();
    r.distributeSpecialFood(300);

    int local = r.getCarrots();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> all;
    if (rank == 0) all.resize(size);

    MPI_Gather(&local, 1, MPI_INT, rank == 0 ? all.data() : nullptr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "\n[ NON-NEGATIVITY TEST ]\n";
        for (int c : all) {
            if (c <= 0) {
                std::cerr << "[FAILED] Negative number of carrots\n";
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        std::cout << "[OK] No negative numbers of carrots found, test passed\n";
    }

    MPI_Finalize();
    return 0;
}
