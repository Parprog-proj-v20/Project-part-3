#include <mpi.h>
#include <cassert>
#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();

    int carrots = r.getCarrots();
    assert(carrots >= 1 && carrots <= 4);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    assert(size == 20);

    if (rank == 0) {
        std::cout << "\n[ INITIALIZATION TEST ]\n";
        std::cout << "Num of proccesses (rabbits): 20\n";
        std::cout << "Num of carrots: [1; 4]\n";
        std::cout << "[OK] Initialization test passed\n";
    }

    MPI_Finalize();
    return 0;
}
