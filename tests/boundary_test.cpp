#include <iostream>
#include "Rabbits.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    Rabbits r;
    r.initialize();

    int before = r.getCarrots();

    r.distributeSpecialFood(0);
    r.exchangeWithNeighbors(0);

    int after = r.getCarrots();

    if (before != after) {
        std::cerr << "[FAILED] Boundary test didn't pass\n";
        return 1;
    }

    if (r.getRank() == 0) {
        std::cout << "\n[ BOUNDARY TEST ]\n";
        std::cout << "[OK] Boundary test (distributed 0 carrots, 0 exchanges made) passed\n";
    }

    MPI_Finalize();
    return 0;
}
