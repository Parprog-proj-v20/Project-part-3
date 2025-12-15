#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    double start = MPI_Wtime();

    Rabbits r;
    r.initialize();
    r.distributeSpecialFood(300);
    r.exchangeWithNeighbors(10);
    r.collectAndCalculateVariance();

    double end = MPI_Wtime();
    double elapsed = end - start;

    double max_time;
    MPI_Reduce(&elapsed, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (r.getRank() == 0) {
        std::cout << "\n[ DEADLOCK TEST ]\n";
        std::cout << "Execution time: " << max_time << " sec\n";

        if (max_time > 10.0) {
            std::cout << "[FAILED] More than 10 seconds passed; possible deadlock!\n";
            return 1;
        }
        else {
            std::cout << "[OK] No deadlock detected\n";
        }
    }

    MPI_Finalize();
    return 0;
}