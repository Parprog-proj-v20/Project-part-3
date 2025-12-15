#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();

    int before = r.getCarrots();
    int total_before;
    MPI_Reduce(&before, &total_before, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    r.distributeSpecialFood(300);

    int after = r.getCarrots();
    int total_after;
    MPI_Reduce(&after, &total_after, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (r.getRank() == 0) {
        std::cout << "\n[ DISTRIBUTION TEST ]\n";
        if (after >= 0) std::cout << "Total carrots after is positive\n";
        else {
            std::cerr << "[FAILED] Total carrots after is negative\n";
            return 1;
        }

        std::cout << "(Total carrots after = total carrots before + 300)\n";
        std::cout << "Total carrots before: " << total_before << std::endl;
        std::cout << "Total carrots after: " << total_after << std::endl;

        if (total_after != total_before + 300) {
            std::cerr << "[FAILED] Distribution test NOT passed\n";
            return 1;
        }
        else std::cout << "[OK] Distribution test passed\n";
    }

    MPI_Finalize();
    return 0;
}
