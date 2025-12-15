#include "Rabbits.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <iomanip>
#include <queue> 

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

/**
 * @brief Инициализация параметров каждого процесса-кролика
 * 
 * Устанавливает случайное начальное количество моркови (от 1 до 4 кг),
 * определяет левого и правого соседей в кольцевой топологии.
 * Использует барьер MPI для синхронизации всех процессов перед началом работы.
 */
void Rabbits::initialize() {
    srand(time(NULL) + rank);
    carrots = (rand() % 4) + 1;

    left_neighbor = (rank - 1 + size) % size;
    right_neighbor = (rank + 1) % size;

    MPI_Barrier(MPI_COMM_WORLD);
}

/**
 * @brief Распределение спецпайка от процесса 0 к остальным процессам
 * 
 * @param total_food Общее количество моркови для распределения 
 */
void Rabbits::distributeSpecialFood(int total_food) {
    if (rank == 0) {
        std::cout << "Starting distribution of " << total_food << " kg" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int distributed = 0;    // Счетчик распределенной еды (только для процесса 0)
    int my_requests = 0;    // Счетчик отправленных запросов (для остальных процессов)
    int my_grants = 0;      // Счетчик полученных удовлетворенных запросов

    srand(time(NULL) + rank * 1000);

    if (rank == 0) {
        std::queue<int> request_queue;
        bool requests_active = true;

        while (distributed < total_food) {
            MPI_Status status;
            int has_request = 0;
            MPI_Iprobe(MPI_ANY_SOURCE, REQUEST_TAG, MPI_COMM_WORLD, &has_request, &status);

            while (has_request) {
                int request;
                MPI_Recv(&request, 1, MPI_INT, status.MPI_SOURCE,
                    REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                request_queue.push(status.MPI_SOURCE);

                MPI_Iprobe(MPI_ANY_SOURCE, REQUEST_TAG, MPI_COMM_WORLD, &has_request, &status);
            }

            while (!request_queue.empty() && distributed < total_food) {
                int rabbit = request_queue.front();
                request_queue.pop();

                int response = 1;
                if (rabbit == 0) {
                    carrots++; 
                }
                else {
                    MPI_Send(&response, 1, MPI_INT, rabbit, RESPONSE_TAG, MPI_COMM_WORLD);
                }

                distributed++;
                if (distributed % 100 == 0) {
                    std::cout << "Distributed: " << distributed << "/" << total_food
                        << " kg" << std::endl;
                }
            }

            if ((rand() % 10) < 3 && distributed < total_food) {  // 30% шанс
                carrots++;
                distributed++;
            }

            double wait_start = MPI_Wtime();
            while (MPI_Wtime() - wait_start < 0.00001) {}
        }

        for (int i = 1; i < size; i++) {
            int end_signal = -1;
            MPI_Send(&end_signal, 1, MPI_INT, i, END_TAG, MPI_COMM_WORLD);
        }

    }
    else {
        bool finished = false;
        srand(time(NULL) + rank * 1000);

        while (!finished) {
            if ((rand() % 100) < 20) { 
                my_requests++;

                int request = 1;
                MPI_Request send_req;
                MPI_Isend(&request, 1, MPI_INT, 0, REQUEST_TAG,
                    MPI_COMM_WORLD, &send_req);

                bool got_response = false;
                double start_time = MPI_Wtime();

                while (!got_response && (MPI_Wtime() - start_time < 0.1)) {
                    MPI_Status status;
                    int has_response = 0;
                    MPI_Iprobe(0, RESPONSE_TAG, MPI_COMM_WORLD, &has_response, &status);

                    if (has_response) {
                        int response;
                        MPI_Recv(&response, 1, MPI_INT, 0, RESPONSE_TAG,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        if (response == 1) {
                            carrots++;
                            my_grants++;
                        }
                        got_response = true;
                    }
                }

                MPI_Request_free(&send_req);
            }

            MPI_Status status;
            int has_end = 0;
            MPI_Iprobe(0, END_TAG, MPI_COMM_WORLD, &has_end, &status);

            if (has_end) {
                int end_signal;
                MPI_Recv(&end_signal, 1, MPI_INT, 0, END_TAG,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                finished = true;
            }

            double wait_start = MPI_Wtime();
            while (MPI_Wtime() - wait_start < 0.0001) {}
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    // Сбор статистики по всем процессам
    std::vector<int> all_carrots(size);
    MPI_Gather(&carrots, 1, MPI_INT, all_carrots.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int total = 0;
        for (int i = 0; i < size; i++) {
            total += all_carrots[i];
        }
        std::cout << "Total after distribution: " << total << " kg" << std::endl;
    }
}

/**
 * @brief Обмен морковью между соседними процессами в кольцевой топологии
 * 
 * Выполняется в два этапа за одну итерацию: сначала с левым, затем с правым соседом.
 *
 * @param exchanges Количество итераций обмена
 */
void Rabbits::exchangeWithNeighbors(int exchanges) {
    for (int i = 0; i < exchanges; i++) {
        int left_carrots;
// Одновременная отправка своего количества и получение от правого соседа
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

/**
 * @brief Сбор данных от всех процессов и вычисление статистики
 * 
 * Процесс 0 собирает количество моркови у всех процессов, вычисляет
 * среднее значение, дисперсию и стандартное отклонение распределения.
 * 
 * Выводит результаты только на процессе 0.
 */
void Rabbits::collectAndCalculateVariance() {
    std::vector<int> all_carrots(size);
    
    // Используем Gather вместо Send/Recv
    MPI_Gather(&carrots, 1, MPI_INT, all_carrots.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
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
}

int Rabbits::getCarrots() const {
    return carrots;
}

int Rabbits::getRank() const {
    return rank;
}


