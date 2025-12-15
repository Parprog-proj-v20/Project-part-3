#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int food[] = { -100, 500, 1000, 2000 };
    int exchanges[] = { -10, 20, 40, 75 };

    for (int f : food) {
        for (int ex : exchanges) {
            Rabbits r;
            r.initialize();

            double t0 = MPI_Wtime();
            r.distributeSpecialFood(f);
            r.exchangeWithNeighbors(ex);
            double t1 = MPI_Wtime();

            double elapsed = t1 - t0;
            double max_elapsed;
            MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

            if (r.getRank() == 0) {
                std::cout << "\n[ VALUES TEST ]\n";
                std::cout << "Carrots: " << f << "kg\nExchanges: " << ex << "\n";
                std::cout << "Time: " << max_elapsed << " sec\n" << std::endl;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
