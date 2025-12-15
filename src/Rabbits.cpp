#include "Rabbits.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

const int REQUEST_TAG = 0;    // Запрос моркови
const int RESPONSE_TAG = 1;   // Ответ с морковью
const int END_TAG = 2;

Rabbits::Rabbits() : rank(0), size(0), carrots(0),
left_neighbor(0), right_neighbor(0) {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
}

Rabbits::~Rabbits() {}

void Rabbits::initialize() {
    srand(time(NULL) + rank);
    carrots = (rand() % 4) + 1;

    left_neighbor = (rank - 1 + size) % size;
    right_neighbor = (rank + 1) % size;

    MPI_Barrier(MPI_COMM_WORLD);
}

void Rabbits::distributeSpecialFood(int total_food) {
    if (rank == 0) {
        std::cout << "Starting special food distribution (" << total_food << " kg)" << std::endl;
        std::cout << "Rabbits will request carrots multiple times..." << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int distributed = 0;  // Сколько уже раздали
    int my_requests = 0;  // Сколько раз заяц просил
    int my_grants = 0;    // Сколько раз зайцу дали

    srand(time(NULL) + rank);

    while (distributed < total_food) {
        int wants_carrots = (rand() % 2) == 0;

        if (wants_carrots) {
            my_requests++;
            
            if (rank == 0) {
                if (distributed < total_food) {
                    carrots++;
                    distributed++;
                    my_grants++;

                    if (distributed % 100 == 0) {
                        std::cout << "Distributed: " << distributed << "/" << total_food
                            << " kg" << std::endl;
                    }
                }
            }
            else {

                int request = 1;
                MPI_Send(&request, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

                // Ждем ответ с таймаутом
                int response;
                MPI_Status status;
                int got_response = 0;

                // Пробуем получить ответ несколько раз
                for (int attempt = 0; attempt < 10; attempt++) {
                    int flag;
                    MPI_Iprobe(0, 1, MPI_COMM_WORLD, &flag, &status);
                    if (flag) {
                        MPI_Recv(&response, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
                        if (response == 1) {
                            carrots++;
                            my_grants++;
                        }
                        got_response = 1;
                        break;
                    }
                    double wait_start = MPI_Wtime();
                    while (MPI_Wtime() - wait_start < 0.001) {}
                }
            }
        }

        if (rank == 0) {
            for (int check = 0; check < 5; check++) {
                MPI_Status status;
                int flag;

                MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);

                if (flag && distributed < total_food) {
                    int request;
                    MPI_Recv(&request, 1, MPI_INT, status.MPI_SOURCE, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    int response = 1;
                    MPI_Send(&response, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);

                    distributed++;

                    if (distributed % 100 == 0) {
                        std::cout << "Distributed: " << distributed << "/" << total_food
                            << " kg" << std::endl;
                    }
                }
                else {
                    break; 
                }
            }
        }

        int global_distributed;
        MPI_Allreduce(&distributed, &global_distributed, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        distributed = global_distributed;

        if (distributed >= total_food) {
            break;
        }

        double wait_start = MPI_Wtime();
        while (MPI_Wtime() - wait_start < 0.0005) {}

        MPI_Barrier(MPI_COMM_WORLD);
    }
    if (rank == 0) {
        std::cout << "\nDistribution completed, sending termination signals..." << std::endl;

        // Обрабатываем возможные оставшиеся запросы перед отправкой сигналов
        double cleanup_start = MPI_Wtime();
        while (MPI_Wtime() - cleanup_start < 0.05) {
            MPI_Status status;
            int has_request = 0;
            MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &has_request, &status);

            if (has_request) {
                int request;
                MPI_Recv(&request, 1, MPI_INT, status.MPI_SOURCE, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Отправляем 0 так как распределение завершено
                int response = 0;
                MPI_Send(&response, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
            }
        }

        for (int i = 1; i < size; i++) {
            int end_signal = -1;
            MPI_Send(&end_signal, 1, MPI_INT, i, 2, MPI_COMM_WORLD); 
        }

        std::cout << "Termination signals sent" << std::endl;
    }
    else {
        int finished = 0;
        while (!finished) {
            int has_end_signal = 0;
            MPI_Status status;
            MPI_Iprobe(0, 2, MPI_COMM_WORLD, &has_end_signal, &status);

            if (has_end_signal) {
                int end_signal;
                MPI_Recv(&end_signal, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                finished = 1;
                break;
            }

            double wait_start = MPI_Wtime();
            while (MPI_Wtime() - wait_start < 0.001) {} 
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "\n=== SPECIAL FOOD DISTRIBUTION COMPLETED ===" << std::endl;
        std::cout << "Total distributed: " << distributed << " kg" << std::endl;
    }

    std::vector<int> all_requests(size);
    std::vector<int> all_grants(size);
    std::vector<int> all_carrots(size);

    MPI_Gather(&my_requests, 1, MPI_INT, all_requests.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&my_grants, 1, MPI_INT, all_grants.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&carrots, 1, MPI_INT, all_carrots.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "\n=== DISTRIBUTION STATISTICS ===" << std::endl;
        std::cout << "Rabbit | Requests | Granted | Success Rate | Total Carrots" << std::endl;

        int total_requests = 0;
        int total_grants = 0;

        for (int i = 0; i < size; i++) {
            double success_rate = (all_requests[i] > 0) ?
                (100.0 * all_grants[i] / all_requests[i]) : 0.0;

            std::cout << std::setw(6) << i << " | "
                << std::setw(8) << all_requests[i] << " | "
                << std::setw(7) << all_grants[i] << " | "
                << std::setw(11) << std::fixed << std::setprecision(1) << success_rate << "% | "
                << std::setw(13) << all_carrots[i] << std::endl;

            total_requests += all_requests[i];
            total_grants += all_grants[i];
        }

        double overall_success_rate = (total_requests > 0) ?
            (100.0 * total_grants / total_requests) : 0.0;

        std::cout << "TOTAL  | "
            << std::setw(8) << total_requests << " | "
            << std::setw(7) << total_grants << " | "
            << std::setw(11) << std::fixed << std::setprecision(1)
            << overall_success_rate << "% | "
            << "             " << std::endl;
    }
    else {
        MPI_Gather(&my_requests, 1, MPI_INT, nullptr, 0, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gather(&my_grants, 1, MPI_INT, nullptr, 0, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gather(&carrots, 1, MPI_INT, nullptr, 0, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
}

void Rabbits::exchangeWithNeighbors(int exchanges) {
    for (int i = 0; i < exchanges; i++) {
        int left_carrots;

        MPI_Sendrecv(&carrots, 1, MPI_INT, left_neighbor, 0,
            &left_carrots, 1, MPI_INT, right_neighbor, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (left_carrots < carrots && carrots > 0) {
            carrots--;
            int gift = 1;
            MPI_Send(&gift, 1, MPI_INT, left_neighbor, 1, MPI_COMM_WORLD);
        }
        else {
            int zero = 0;
            MPI_Send(&zero, 1, MPI_INT, left_neighbor, 1, MPI_COMM_WORLD);
        }

        int received_from_right;
        MPI_Recv(&received_from_right, 1, MPI_INT, right_neighbor, 1,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        carrots += received_from_right;

        int right_carrots;

        MPI_Sendrecv(&carrots, 1, MPI_INT, right_neighbor, 2,
            &right_carrots, 1, MPI_INT, left_neighbor, 2,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (right_carrots < carrots && carrots > 0) {
            carrots--;
            int gift = 1;
            MPI_Send(&gift, 1, MPI_INT, right_neighbor, 3, MPI_COMM_WORLD);
        }
        else {
            int zero = 0;
            MPI_Send(&zero, 1, MPI_INT, right_neighbor, 3, MPI_COMM_WORLD);
        }

        int received_from_left;
        MPI_Recv(&received_from_left, 1, MPI_INT, left_neighbor, 3,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        carrots += received_from_left;

        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void Rabbits::collectAndCalculateVariance() {
    if (rank == 0) {
        std::vector<int> all_carrots(size);
        all_carrots[0] = carrots;

        for (int i = 1; i < size; i++) {
            MPI_Recv(&all_carrots[i], 1, MPI_INT, i, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        std::cout << "\nCarrot distribution after exchange:" << std::endl;
        for (int i = 0; i < size; i++) {
            std::cout << "Rabbit " << i << ": " << all_carrots[i] << " kg" << std::endl;
        }

        double sum = 0;
        for (int i = 0; i < size; i++) {
            sum += all_carrots[i];
        }
        double mean = sum / size;

        double variance = 0;
        for (int i = 0; i < size; i++) {
            variance += (all_carrots[i] - mean) * (all_carrots[i] - mean);
        }
        variance /= size;

        std::cout << "\nResults:" << std::endl;
        std::cout << "Mean carrot count: " << mean << " kg" << std::endl;
        std::cout << "Variance: " << variance << std::endl;
        std::cout << "Standard deviation: " << sqrt(variance) << std::endl;
        std::cout << "Total carrots: " << sum << " kg" << std::endl;

    }
    else {
        MPI_Send(&carrots, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}

int Rabbits::getCarrots() const {
    return carrots;
}

int Rabbits::getRank() const {
    return rank;
}
