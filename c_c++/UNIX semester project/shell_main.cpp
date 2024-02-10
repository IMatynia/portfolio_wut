//
// Created by imat on 21.12.23.
//

#include "src/DummyTuplespace.h"
#include "src/RealTuplespace.h"
#include "src/Shell.h"
#include <spdlog/spdlog.h>

#include <iostream>

void usage() {
    std::cout << "shell [demon path] [number of buckets] [bucket size] (--clean) (--debug)\n";
    std::cout << "demon path - absolute path to the demon executable\n"
                 "number of buckets - same as used in the shell\n"
                 "bucket size - same as used in shell\n"
                 "--clean - optional flag for clean output, useful for piping data\n"
                 "--log - optional debug printing\n";
}

int proper_main(int argc, char *argv[]) {
    bool rawOutput = false;
    std::string path;
    if (argc < 3) {
        usage();
        return 1;
    }

    path = argv[1];
    int buckets = std::stoi(std::string(argv[2]));
    int buckets_size = std::stoi(std::string(argv[3]));

    for (int arg = 4; argv[arg]; ++arg) {
        std::string flag = argv[arg];
        if (flag == "--clean")
            rawOutput = true;
        else if (flag == "--log")
            spdlog::set_level(spdlog::level::debug);
    }
    AbstractTuplespaceApi *tuplespaceApi;
    try {
        tuplespaceApi = new RealTuplespace(path, getpid(), buckets, buckets_size);
        tuplespaceApi->init();
    }
    catch (std::exception &e) {
        std::cout << "Failed to initialize tuplespace!\n";
        perror(e.what());
        exit(EXIT_FAILURE);
    }

    if (!rawOutput)
        std::cout << "Tuple space initialized" << std::endl;
    Shell shell(tuplespaceApi, std::cin, std::cout, rawOutput);
    shell.registerSignalHandlers();
    shell.mainLoop();
    tuplespaceApi->close();
    delete tuplespaceApi;
    return 0;
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::warn);
    try {
        return proper_main(argc, argv);
    } catch (const std::exception &e) {
        SPDLOG_ERROR(e.what());
    }
}
