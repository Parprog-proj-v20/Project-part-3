#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();

    int carrots = r.getCarrots();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        std::cout << "\n[ INITIALIZATION TEST ]\n";
        if (size == 20) std::cout << "Num of proccesses (rabbits) is 20\n";
        else {
            std::cerr << "[FAILED] Num of proccesses (rabbits) is not 20\n";
            return 1;
        }
        if (carrots >= 0) std::cout << "Num of carrots is in [1; 4]\n";
        else {
            std::cerr << "[FAILED] Num of carrots is out of [1; 4]\n";
            return 1;
        }
        std::cout << "[OK] Initialization test passed\n";
    }

    MPI_Finalize();
    return 0;
}
