//
// Created by imat on 16.01.24.
//

#ifndef TUPLESPACE_UTILS_H
#define TUPLESPACE_UTILS_H

#include <spdlog/spdlog.h>
#include "Tuple.h"
#include "Demon.h"
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string>
#include <signal.h>
#include <memory>
#include <iostream>
#include <fcntl.h>
#include <exception>
#include <cstddef>
#include "tuple_list.hpp"
#include "new_tuple_queue.hpp"
#include "RequestQueueSharedMemoryMap.h"
#include <bits/types/struct_timespec.h>

static sig_atomic_t quit = 0;

static timespec ms_to_timespec(long long ms);

static void quit_signal_handler(int signal);

static timespec ms_to_timespec(long long int ms) {
    int seconds = ms / 1000;
    ms = ms - 1000 * seconds;
    timespec ts{
        .tv_sec = seconds,
        .tv_nsec = 1'000'000 * ms,
    };
    return ts;
}

static void quit_signal_handler(int signal) {
    (void) signal;
    quit = 1;
}

#endif // TUPLESPACE_UTILS_H
