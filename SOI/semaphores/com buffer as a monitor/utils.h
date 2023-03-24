#ifndef LAB4_UTILS_H
#define LAB4_UTILS_H

#include <random>
#include <csignal>

std::mt19937 rng;
std::uniform_real_distribution<float> uniform(-1.0f, 1.0f);

float rand_fraction;

void init_rng(uint32_t seed, float new_rand_fraction)
{
    rng.seed(seed);
    rand_fraction = new_rand_fraction;
}

void rand_sleep(int base_sleep) {
    int rand_num = base_sleep + int(float(base_sleep) * uniform(rng) * rand_fraction);
    usleep(rand_num);
}

#endif //LAB4_UTILS_H
