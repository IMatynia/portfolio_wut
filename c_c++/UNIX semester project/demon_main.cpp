#include "src/Demon.h"
#include <exception>
#include <iostream>
#include <spdlog/spdlog.h>

static void usage() {
    std::cout << "demon [number of buckets] [bucket size] (--log)\n";
    std::cout << "number of buckets - in tuple space\n"
                 "bucket size - in tuple space\n"
                 "--log - optional log printing\n";
}

int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::warn);

    if (argc < 3) {
        usage();
        return 1;
    }

    int buckets = std::stoi(std::string(argv[1]));
    int buckets_size = std::stoi(std::string(argv[2]));

    for (int arg = 3; argv[arg]; ++arg) {
        std::string flag = argv[arg];
        if (flag == "--log")
            spdlog::set_level(spdlog::level::debug);
        else {
            usage();
            return 1;
        }
    }
    try {
        Demon demon(argv[0], buckets, buckets_size);
        demon.start();
    } catch (const std::exception &e) {
        SPDLOG_ERROR(e.what());
    }
    return 0;
}
