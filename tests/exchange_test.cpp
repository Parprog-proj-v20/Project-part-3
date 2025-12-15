#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();
    r.distributeSpecialFood(300);

    int before = r.getCarrots();
    int total_before;
    MPI_Reduce(&before, &total_before, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    r.exchangeWithNeighbors(10);

    int after = r.getCarrots();
    int total_after;
    MPI_Reduce(&after, &total_after, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (r.getRank() == 0) {
        std::cout << "\n[ EXCHANGE TEST ]\n";
        if (after >= 0) std::cout << "Total carrots after is positive\n";
        else {
            std::cerr << "[FAILED] Total carrots after is negative\n";
            return 1;
        }

        std::cout << "(Total carrots before exchange = total carrots after echange)\n";
        std::cout << "Total carrots before exchange: " << total_before << std::endl;
        std::cout << "Total carrots after exchange: " << total_after << std::endl;

        if (total_after != total_before) {
            std::cerr << "[FAILED] Exchange test NOT passed\n";
            return 1;
        }
        else std::cout << "[OK] Exchange test passed\n";
    }

    MPI_Finalize();
    return 0;
}
