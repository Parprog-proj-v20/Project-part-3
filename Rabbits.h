#ifndef RABBITS_H
#define RABBITS_H

#include <mpi.h>

class Rabbits {
private:
    int rank;            // ID зайца
    int size;            // Всего зайцев
    int carrots;         // Сколько моркови у текущего зайца

    int left_neighbor;   // ID левого соседа
    int right_neighbor;  // ID правого соседа

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