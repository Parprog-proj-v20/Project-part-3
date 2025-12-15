#ifndef RABBITS_H
#define RABBITS_H

#include <mpi.h>
//
class Rabbits {
private:
    int rank;            // ID çàéöà
    int size;            // Âñåãî çàéöåâ
    int carrots;         // Ñêîëüêî ìîðêîâè ó òåêóùåãî çàéöà

    int left_neighbor;   // ID ëåâîãî ñîñåäà
    int right_neighbor;  // ID ïðàâîãî ñîñåäà

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
