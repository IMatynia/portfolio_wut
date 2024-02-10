#include <iostream>
#include <sys/mman.h>
#include "producer.h"
#include "consumer.h"

#define TIMEOUT 5

int main(int ac, const char *av[]) {
    float new_rand_fraction = 0.5;
    //default times
    int dough_prod_time = 500000;
    int meat_prod_time = 500000;
    int cheese_prod_time = 500000;
    int cabb_prod_time = 500000;
    int meat_cons_time = 500000;
    int cheese_cons_time = 500000;
    int cabb_cons_time = 500000;
    //default producer/consumer counts
    int n_dough_prod = 1;
    int n_meat_prod = 1;
    int n_cheese_prod = 1;
    int n_cabb_prod = 1;
    int n_meat_cons = 1;
    int n_cheese_cons = 1;
    int n_cabb_cons = 1;

    if (ac == 16) {
        // losowosc czasu ARG 1
        new_rand_fraction = atof(av[1]);

        // czasy produkcji ARG 2-8
        dough_prod_time = atoi(av[2]);
        meat_prod_time = atoi(av[3]);
        cheese_prod_time = atoi(av[4]);
        cabb_prod_time = atoi(av[5]);
        meat_cons_time = atoi(av[6]);
        cheese_cons_time = atoi(av[7]);
        cabb_cons_time = atoi(av[8]);

        // argument ilości producentów i konsumentów ARG 9-15
        n_dough_prod = atoi(av[9]);
        n_meat_prod = atoi(av[10]);
        n_cheese_prod = atoi(av[11]);
        n_cabb_prod = atoi(av[12]);
        n_meat_cons = atoi(av[13]);
        n_cheese_cons = atoi(av[14]);
        n_cabb_cons = atoi(av[15]);
    }

    // declaring shared memory
    IngredientBuffer *buf_dough;
    buf_dough = static_cast<IngredientBuffer *>(mmap(nullptr, sizeof *buf_dough, PROT_READ | PROT_WRITE,
                                                     MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    IngredientBuffer *buf_meat;
    buf_meat = static_cast<IngredientBuffer *>(mmap(nullptr, sizeof *buf_meat, PROT_READ | PROT_WRITE,
                                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    IngredientBuffer *buf_cheese;
    buf_cheese = static_cast<IngredientBuffer *>(mmap(nullptr, sizeof *buf_cheese, PROT_READ | PROT_WRITE,
                                                      MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    IngredientBuffer *buf_cabbage;
    buf_cabbage = static_cast<IngredientBuffer *>(mmap(nullptr, sizeof *buf_cabbage, PROT_READ | PROT_WRITE,
                                                       MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    *buf_dough = IngredientBuffer();
    *buf_meat = IngredientBuffer();
    *buf_cheese = IngredientBuffer();
    *buf_cabbage = IngredientBuffer();

    std::vector<pid_t> all_childern;

    create_n_producers(buf_dough, "ciasto", dough_prod_time, n_dough_prod, new_rand_fraction, all_childern);
    create_n_producers(buf_meat, "mieso", meat_prod_time, n_meat_prod, new_rand_fraction, all_childern);
    create_n_producers(buf_cheese, "twarog", cheese_prod_time, n_cheese_prod, new_rand_fraction,all_childern);
    create_n_producers(buf_cabbage, "kapusta", cabb_prod_time, n_cabb_prod, new_rand_fraction, all_childern);

    create_n_consumers(buf_dough, buf_meat, "pierog z miesem", meat_cons_time, n_meat_cons, new_rand_fraction,all_childern);
    create_n_consumers(buf_dough, buf_cheese, "pierog z twarogiem", cheese_cons_time, n_cheese_cons, new_rand_fraction, all_childern);
    create_n_consumers(buf_dough, buf_cabbage, "pierog z kapusta", cabb_cons_time, n_cabb_cons, new_rand_fraction, all_childern);

    sleep(TIMEOUT);
    std::cout << "Timed out!\n";
    for(pid_t pid : all_childern)
    {
        kill(pid, SIGKILL);
    }
    return 0;
}
