#ifndef RABBITS_H
#define RABBITS_H

#include <mpi.h>

class Rabbits {
private:
    int rank;
    int size;
    int carrots;

    int left_neighbor;
    int right_neighbor;

public:
    Rabbits();
    ~Rabbits();

    void initialize();                    
    void distributeSpecialFood(int total_food); 
    void exchangeWithNeighbors(int exchanges);  
    void collectAndCalculateVariance();  
    int getCarrots() const;
    int getRank() const;
};


#endif
