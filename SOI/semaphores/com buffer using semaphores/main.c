#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define BUFFER_SIZE         10
#define TIMEOUT             300

#define S_FULL            0
#define S_EMPTY           1
#define S_MUTEX           2

double rand_fraction = 0.2;

void down(int sem_id, ushort which_one) {
    struct sembuf sem_op;
    sem_op.sem_num = which_one;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;

    if (semop(sem_id, &sem_op, 1) < 0) {
        perror("Error in locking semaphore");
        exit(1);
    }
}

void up(int sem_id, ushort which_one) {
    struct sembuf sem_op;
    sem_op.sem_num = which_one;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;

    if (semop(sem_id, &sem_op, 1) < 0) {
        perror("Error in locking semaphore");
        exit(1);
    }
}

struct Data_s {
    int batch;
    int producer_id;
    float amount;
    char *ingredient_name;
};
typedef struct Data_s Data;

void data_to_string(Data *data, char *destination) {
    sprintf(destination, "%0.1fg %s (B_%d_%d)", data->amount, data->ingredient_name, data->producer_id, data->batch);
}

struct Buffer_s {
    Data _data[BUFFER_SIZE];
    int _top;
    int sem_group;
};
typedef struct Buffer_s Buffer;

void buffer_init(Buffer *buf, int sem_group_id) {
    buf->_top = 0;
    buf->sem_group = sem_group_id;

    if (semctl(sem_group_id, S_EMPTY, SETVAL, BUFFER_SIZE - 1) < 0 ||
        semctl(sem_group_id, S_FULL, SETVAL, 0) < 0 ||
        semctl(sem_group_id, S_MUTEX, SETVAL, 1) < 0) {
        perror("Error initializing semaphores!");
        exit(1);
    }
}

void buffer_insert(Buffer *buf, Data data) {
    if (buf->_top == BUFFER_SIZE) {
        perror("Can't insert into a full buffer");
        exit(1);
    }
    buf->_data[buf->_top] = data;
    buf->_top++;
}

Data buffer_pop(Buffer *buf) {
    buf->_top--;
    if (buf->_top == -1) {
        perror("Cant pop from an empty buffer");
        exit(1);
    }
    return buf->_data[buf->_top];
}

void rand_sleep(int base_sleep) {
    int rand_num = (int) (2 * (rand() / (double) RAND_MAX - 0.5) * rand_fraction * base_sleep + base_sleep);
    usleep(rand_num);
}

Data produce_item(int *i, char *ingredient, float amount, int time_taken, int producer_id) {
    char *current_data_str = malloc(200);

    Data data;
    data.batch = *i;
    data.amount = amount;
    data.ingredient_name = ingredient;
    data.producer_id = producer_id;

    data_to_string(&data, current_data_str);
    printf("%28s_%d [P] Start producing\t %s\n", ingredient, producer_id, current_data_str);

    rand_sleep(time_taken);
    printf("%28s_%d [P] Production done\t %s\n", ingredient, producer_id, current_data_str);
    (*i)++;
    return data;
}

void consume_item(Data *dough, Data *filling, char *pierog_type, int time_taken, int consumer_id) {
    char *dough_str = malloc(200);
    char *filling_str = malloc(200);

    data_to_string(dough, dough_str);
    data_to_string(filling, filling_str);

    printf("%28s_%d [C] Combining (%s) and (%s) to make (%s)\n", pierog_type, consumer_id, dough_str, filling_str,
           pierog_type);
    rand_sleep(time_taken);
    printf("%28s_%d [C] Made %s\n", pierog_type, consumer_id, pierog_type);
}

void producer(Buffer *buf, char *ingredient_type, int time_taken, int id) {
    Data data;
    int i = 0;
    while (1) {
        data = produce_item(&i, ingredient_type, 10.0f, time_taken, id);
        printf("%28s_%d [P] Wait for space in buffer...\n", ingredient_type, id);
        down(buf->sem_group, S_EMPTY);
        down(buf->sem_group, S_MUTEX);
        buffer_insert(buf, data);
        printf("%28s_%d [P] Added to buffer!\n", ingredient_type, id);
        up(buf->sem_group, S_MUTEX);
        up(buf->sem_group, S_FULL);
    }
}

void consumer(Buffer *dough_buffer, Buffer *filling_buffer, char *pierogi_type, int time_taken, int id) {
    Data dough, filling;
    while (1) {
        printf("%28s_%d [C] Waiting for ingredients...\n", pierogi_type, id);
        down(dough_buffer->sem_group, S_FULL);
        down(filling_buffer->sem_group, S_FULL);

        down(dough_buffer->sem_group, S_MUTEX);
        down(filling_buffer->sem_group, S_MUTEX);

        dough = buffer_pop(dough_buffer);
        filling = buffer_pop(filling_buffer);

        up(filling_buffer->sem_group, S_MUTEX);
        up(dough_buffer->sem_group, S_MUTEX);

        up(filling_buffer->sem_group, S_EMPTY);
        up(dough_buffer->sem_group, S_EMPTY);

        consume_item(&dough, &filling, pierogi_type, time_taken, id);
    }
}

void create_n_consumers(Buffer *dough_buffer, Buffer *ingredient_buffer, char *pierogi_type, int time_taken, int n) {
    pid_t child_pid, my_pid;
    for (int i = 0; i < n; i++) {
        child_pid = fork();
        if (child_pid < 0) {
            perror("Error in forking to a consumer");
            exit(1);
        }

        if (child_pid == 0) {
            my_pid = getpid();
            printf("> CONSUMER (%s_%d) PID: %d\n", pierogi_type, i, my_pid);
            consumer(dough_buffer, ingredient_buffer, pierogi_type, time_taken, i);
        }
    }
}

void create_n_producers(Buffer *ingredient_buffer, char *ingredient_type, int time_taken, int n) {
    pid_t child_pid, my_pid;
    for (int i = 0; i < n; i++) {
        child_pid = fork();
        if (child_pid < 0) {
            perror("Error in forking to a producer");
            exit(1);
        }

        if (child_pid == 0) {
            my_pid = getpid();
            printf("> PRODUCER (%s_%d) PID: %d\n", ingredient_type, i, my_pid);
            producer(ingredient_buffer, ingredient_type, time_taken, i);
        }
    }
}

int main(int ac, const char *av[]) {
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
        rand_fraction = atof(av[1]);

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
    srand(time(0));

    // declaring shared memory
    Buffer *buf_dough;
    buf_dough = mmap(NULL, sizeof *buf_dough, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    Buffer *buf_meat;
    buf_meat = mmap(NULL, sizeof *buf_meat, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    Buffer *buf_cheese;
    buf_cheese = mmap(NULL, sizeof *buf_cheese, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    Buffer *buf_cabbage;
    buf_cabbage = mmap(NULL, sizeof *buf_cabbage, PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // creating semaphore groups
    int dough_sem_id = semget(IPC_PRIVATE, 3, 0666);
    if (dough_sem_id < 0) {
        perror("Error creating semaphore group");
        exit(1);
    }
    int meat_sem_id = semget(IPC_PRIVATE, 3, 0666);
    if (meat_sem_id < 0) {
        perror("Error creating semaphore group");
        exit(1);
    }
    int cheese_sem_id = semget(IPC_PRIVATE, 3, 0666);
    if (cheese_sem_id < 0) {
        perror("Error creating semaphore group");
        exit(1);
    }
    int cabbage_sem_id = semget(IPC_PRIVATE, 3, 0666);
    if (cabbage_sem_id < 0) {
        perror("Error creating semaphore group");
        exit(1);
    }

    // initializing the buffers
    buffer_init(buf_dough, dough_sem_id);
    buffer_init(buf_meat, meat_sem_id);
    buffer_init(buf_cheese, cheese_sem_id);
    buffer_init(buf_cabbage, cabbage_sem_id);

    //creating child procesees
    create_n_producers(buf_dough, "ciasto", dough_prod_time, n_dough_prod);
    create_n_producers(buf_meat, "mieso", meat_prod_time, n_meat_prod);
    create_n_producers(buf_cheese, "twarog", cheese_prod_time, n_cheese_prod);
    create_n_producers(buf_cabbage, "kapusta", cabb_prod_time, n_cabb_prod);

    create_n_consumers(buf_dough, buf_meat, "pierog z miesem", meat_cons_time, n_meat_cons);
    create_n_consumers(buf_dough, buf_cheese, "pierog z twarogiem", cheese_cons_time, n_cheese_cons);
    create_n_consumers(buf_dough, buf_cabbage, "pierog z kapustą", cabb_cons_time, n_cabb_cons);

    sleep(TIMEOUT);
    return 0;
}
