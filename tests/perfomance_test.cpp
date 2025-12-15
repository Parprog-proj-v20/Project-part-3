#include <iostream>
#include <iomanip>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();

    double t_start, t_end;

    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();
    r.distributeSpecialFood(300);
    MPI_Barrier(MPI_COMM_WORLD);
    t_end = MPI_Wtime();

    double dist_time = t_end - t_start;

    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();
    r.exchangeWithNeighbors(10);
    MPI_Barrier(MPI_COMM_WORLD);
    t_end = MPI_Wtime();

    double exch_time = t_end - t_start;

    double max_dist, max_exch;
    MPI_Reduce(&dist_time, &max_dist, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&exch_time, &max_exch, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (r.getRank() == 0) {
        std::cout << "\n[ PERFORMANCE TEST ]\n";
        std::cout << "Total time, max times for distribution and exchange\n";
        std::cout << "Distribution time (max): " << max_dist << " sec\n";
        std::cout << "Exchange time (max): " << max_exch << " sec\n";
        std::cout << "Total time: " << max_dist + max_exch << " sec\n";
    }

    MPI_Finalize();
    return 0;
}
