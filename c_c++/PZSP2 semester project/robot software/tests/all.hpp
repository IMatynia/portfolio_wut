//
// Created by imat on 24.11.23.
//

#ifndef ROBOT_MAPUJACY_ALL_H
#define ROBOT_MAPUJACY_ALL_H

#include "protocolTests.hpp"

using testFunc_t = bool (*)(void);

struct Test_t {
    testFunc_t func;
    const char *name;
};

Test_t all_tests[] = {
        {test_protocol_string_parser, "Protocol parser"},
        {nullptr, nullptr}
};

static int runAll(int, char**) {
    bool success;
    int i=-1;
    while(all_tests[++i].func != nullptr) {
        success = all_tests[i].func();
        if (success) {
            printf("Test [%s] works :)\n", all_tests[i].name);
        } else {
            printf("Test [%s] FAILED :(\n", all_tests[i].name);
        }
    }
    return 0;
}

#endif //ROBOT_MAPUJACY_ALL_H
