#ifndef LAB4_CONSUMER_H
#define LAB4_CONSUMER_H

#include "utils.h"
#include "ingreedients_buffer.h"
#include <iostream>
#include <iomanip>

void consume_item(Data *dough, Data *filling, const char *pierog_type, int time_taken, int consumer_id) {
    std::cout << std::setw(28) << pierog_type << std::setw(0) << "_" << consumer_id << " [C] Combining (" << *dough
              << ") and (" << *filling << ") to make (" << pierog_type << ")\n";
    rand_sleep(time_taken);
    std::cout << std::setw(28) << pierog_type << std::setw(0) << "_" << consumer_id << " [C] Made " << pierog_type
              << std::endl;
}

void consumer(IngredientBuffer *dough_buffer, IngredientBuffer *filling_buffer, const char *pierogi_type, int time_taken,
         int id) {
    Data dough, filling;
    while (1) {
        std::cout << std::setw(28) << pierogi_type << std::setw(0) << "_" << id << " [C] Waiting for ingredients...\n";

        filling = filling_buffer->get_ingredient();
        dough = dough_buffer->get_ingredient();

        consume_item(&dough, &filling, pierogi_type, time_taken, id);
    }
}

void create_n_consumers(IngredientBuffer *dough_buffer, IngredientBuffer *ingredient_buffer, const char *pierogi_type,
                        int time_taken, int n, float new_rand_fraction, std::vector<pid_t> &all_childern) {
    pid_t child_pid, my_pid;
    for (int i = 0; i < n; i++) {
        child_pid = fork();
        if (child_pid < 0) {
            throw std::runtime_error("Error in forking to a consumer");
        }

        if (child_pid == 0) {
            my_pid = getpid();
            printf("> CONSUMER (%s_%d) PID: %d\n", pierogi_type, i, my_pid);
            init_rng(my_pid, new_rand_fraction);
            consumer(dough_buffer, ingredient_buffer, pierogi_type, time_taken, i);
        } else {
            all_childern.push_back(child_pid);
        }
    }
}

#endif //LAB4_CONSUMER_H
