#include <iostream>
#include <iomanip>
#include <string>
#include "Rabbits.h"

#ifdef _WIN32 
#include <windows.h>
#endif
//
void runSimulation() {
    Rabbits rabbit;
    rabbit.initialize();

    std::cout << "Rabbit " << rabbit.getRank() << ": initial carrots = "
        << rabbit.getCarrots() << " kg" << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    rabbit.distributeSpecialFood(300);
    MPI_Barrier(MPI_COMM_WORLD);

    std::cout << "Rabbit " << rabbit.getRank() << ": after distribution = "
        << rabbit.getCarrots() << " kg" << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    rabbit.exchangeWithNeighbors(10);
    MPI_Barrier(MPI_COMM_WORLD);

    std::cout << "Rabbit " << rabbit.getRank() << ": after exchange = "
        << rabbit.getCarrots() << " kg" << std::endl;

    rabbit.collectAndCalculateVariance();
}

int main(int argc, char** argv) {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 20) {
        if (rank == 0) {
            std::cerr << "Error: program must be run with 20 processes!" << std::endl;
            std::cerr << "Use: mpiexec -n 20 rabbits.exe" << std::endl;
            std::cerr << "Current number of processes: " << size << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        std::cout << "\tNumber of rabbits: " << size << std::endl;
    }

    runSimulation();

    MPI_Finalize();
    return 0;

}
