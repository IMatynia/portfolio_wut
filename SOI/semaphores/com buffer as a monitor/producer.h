#ifndef LAB4_PRODUCER_H
#define LAB4_PRODUCER_H

#include "utils.h"
#include "ingreedients_buffer.h"
#include <iostream>
#include <iomanip>

Data produce_item(int *i, const char *ingredient, float amount, int time_taken, int producer_id) {
    Data data = {*i, producer_id, amount, ingredient};

    std::cout << std::setw(28) << ingredient << std::setw(0) << "_" << producer_id << " [P] Start producing\t " << data
              << std::endl;
    rand_sleep(time_taken);
    std::cout << std::setw(28) << ingredient << std::setw(0) << "_" << producer_id << " [P] Production done\t " << data
              << std::endl;
    (*i)++;
    return data;
}

void producer(IngredientBuffer *buf, const char *ingredient_type, int time_taken, int id) {
    Data data;
    int i = 0;
    while (1) {
        data = produce_item(&i, ingredient_type, 10.0f, time_taken, id);
        std::cout << std::setw(28) << ingredient_type << std::setw(0) << "_" << id
                  << " [P] Wait for space in buffer...\n";
        buf->put_ingredient(data);
        std::cout << std::setw(28) << ingredient_type << std::setw(0) << "_" << id << " [P] Added to buffer!\n";
    }
}

void create_n_producers(IngredientBuffer *ingredient_buffer, const char *ingredient_type, int time_taken, int n,
                        float new_rand_fraction, std::vector<pid_t> &all_childern) {
    pid_t child_pid, my_pid;
    for (int i = 0; i < n; i++) {
        child_pid = fork();
        if (child_pid < 0) {
            throw std::runtime_error("Error in forking to a producer");
        }

        if (child_pid == 0) {
            my_pid = getpid();
            printf("> PRODUCER (%s_%d) PID: %d\n", ingredient_type, i, my_pid);
            init_rng(my_pid, new_rand_fraction);
            producer(ingredient_buffer, ingredient_type, time_taken, i);
        } else {
            all_childern.push_back(child_pid);
        }
    }
}

#endif //LAB4_PRODUCER_H
