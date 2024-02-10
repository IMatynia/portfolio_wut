cmake_minimum_required(VERSION 3.26)
project(TupleSpace CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB SOURCES "src/*" "src/*")
file(GLOB TESTS "tests/*")

add_executable(tests ${SOURCES} ${TESTS}
)

target_link_libraries(
        tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)

add_executable(shell shell_main.cpp
        ${SOURCES}
)

add_executable(daemon-test-tool daemon-test-tool.cpp
        ${SOURCES}
    )

add_executable(demon demon_main.cpp
        ${SOURCES}
)

target_compile_options(daemon-test-tool PRIVATE -Wall -Wextra)

include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#10.2.1")
CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY "gabime/spdlog"
    VERSION 1.12.0
    OPTIONS SPDLOG_FMT_EXTERNAL)

target_link_libraries(daemon-test-tool
    spdlog::spdlog
    fmt::fmt
)

target_link_libraries(demon
    spdlog::spdlog
    fmt::fmt
)

target_link_libraries(shell
    spdlog::spdlog
    fmt::fmt
)

target_link_libraries(tests
        spdlog::spdlog
        fmt::fmt
)
