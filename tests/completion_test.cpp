#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    double start = MPI_Wtime();

    Rabbits r;
    r.initialize();
    r.distributeSpecialFood(300);

    double elapsed = MPI_Wtime() - start;

    if (elapsed > 5.0) {
        std::cerr << "[FAILED] Proccess timeout\n";
        return 1;
    }

    if (r.getRank() == 0) {
        std::cout << "\n[ COMPLETION TEST ]\n";
        std::cout << "[OK] Completion test passed, all proccesses finished working\n";
    }

    MPI_Finalize();
    return 0;
}
