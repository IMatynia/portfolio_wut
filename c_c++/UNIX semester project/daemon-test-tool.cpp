#include "spdlog/spdlog.h"
#include "src/new_tuple_queue.hpp"
#include "src/tuple_list.hpp"
#include <getopt.h>
#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <vector>

static int handle_add(ts::TupleList &tl, const std::string &arg);
static int handle_remove(ts::TupleList &tl, const std::string &arg);
static int handle_print(ts::TupleList &tl);
static int handle_enqueue(ts::NewTupleQueue &q, const std::string &arg);
static int handle_dequeue(ts::NewTupleQueue &q);

static void log_tl(ts::TupleList &tl) {
    spdlog::debug("Block list start");
    for (auto it = tl.iter(); it; ++it) {
        spdlog::debug(it->to_string());
    }
    spdlog::debug("Block list end");
}

static void usage() {
    std::cout
        << "-c, --create           Create shared memory and semaphore set\n"
        << "-d, --delete           Delete shared memory and semaphore set\n"
        << "\nTuple List\n"
        << "-a, --add <string>     Add a string\n"
        << "-r, --remove <string>  Remove a string\n"
        << "-p, --print            Print strings\n"
        << "--run                  Run a few operations\n"
        << "\nNew Tuple Queue\n"
        << "--enqueue <int>        Enqueue a number\n"
        << "--dequeue              Dequeue a number\n"
        << "-v, --verbose          Print more logs\n"
        << "    --help             Print this help\n";
}

int proper_main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::info);
    bool create = false;
    bool remove = false;
    bool run = false;
    bool do_add = false;
    bool do_remove = false;
    bool do_print = false;
    bool do_enqueue = false;
    bool do_dequeue = false;
    std::string arg;

    for (;;) {
        static struct option long_options[] = {
            {"create", no_argument, 0, 'c'},
            {"delete", no_argument, 0, 'd'},
            {"run", no_argument, 0, 0},
            {"verbose", no_argument, 0, 'v'},
            {"help", no_argument, 0, 0},
            {"add", required_argument, 0, 'a'},
            {"remove", required_argument, 0, 'r'},
            {"enqueue", required_argument, 0, 0},
            {"dequeue", no_argument, 0, 0},
            {"print", no_argument, 0, 'p'},
            {0, 0, 0, 0}};
        int option_index;
        int c =
            getopt_long(argc, argv, "cdva:r:p", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 0: {
            auto option = std::string(long_options[option_index].name);
            if ("help" == option) {
                usage();
                exit(0);
            } else if ("run" == option) {
                run = true;
                break;
            } else if ("enqueue" == option) {
                do_enqueue = true;
                arg = optarg;
                break;
            } else if ("dequeue" == option) {
                do_dequeue = true;
                break;
            }
            exit(1);
        }
        case 'c':
            create = true;
            break;
        case 'd':
            remove = true;
            break;
        case 'v':
            spdlog::set_level(spdlog::level::debug);
            break;
        case 'a':
            do_add = true;
            arg = optarg;
            break;
        case 'r':
            do_remove = true;
            arg = optarg;
            break;
        case 'p':
            do_print = true;
            break;
        default:
            usage();
            return 1;
        }
    }
    if (optind < argc) {
        std::cerr << "unexpected argument: \"" << argv[optind] << "\"\n";
        usage();
        return 1;
    }
    if (!run && !create && !remove && !do_add && !do_remove && !do_print &&
        !do_enqueue && !do_dequeue) {
        usage();
        return 0;
    }

    ts::TupleList tl(ftok(argv[0], 0), "/tuple-space-tuples", create, remove,
                     256, 1024);
    ts::NewTupleQueue q(ftok(argv[0], 1), "/tuple-space-new-tuple-queue",
                        create, remove, 16);
    if (do_add)
        return handle_add(tl, arg);
    if (do_remove)
        return handle_remove(tl, arg);
    if (do_print)
        return handle_print(tl);
    if (do_enqueue)
        return handle_enqueue(q, arg);
    if (do_dequeue)
        return handle_dequeue(q);
    if (!run)
        return 0;
    log_tl(tl);
    std::vector<std::string> strings{
        "this is the first string", "another one", "the third one", "4th",
        "fifth - the last one",
    };
    for (auto &s : strings) {
        {
            auto it = tl.iter();
            while (it && !it->is_free())
                ++it;
            it.alloc(s.size() + 1);
            std::memcpy(it->data(), s.c_str(), s.size() + 1);
        }
        log_tl(tl);
    }
    for (auto it = tl.iter(); it; ++it) {
        if (it->is_used())
            std::cout << std::string(it->data()) << std::endl;
    }
    {
        auto it = tl.iter();
        ++it;
        it.free();
        ++it;
        ++it;
        it.free();
    }
    log_tl(tl);
    {
        auto it = tl.iter();
        ++it;
        ++it;
        it.free();
    }
    log_tl(tl);
    for (auto it = tl.iter(); it; ++it) {
        if (it->is_used())
            it.free();
    }
    log_tl(tl);
    return 0;
}

int main(int argc, char **argv) {
    try {
        proper_main(argc, argv);
    } catch (const std::exception &e) {
        spdlog::error(e.what());
        return 1;
    }
}

int handle_add(ts::TupleList &tl, const std::string &arg) {
    {
        const auto data_size = arg.size() + 1;
        auto it = tl.iter();
        while (it && (it->is_used() || it->max_data_size() < data_size))
            ++it;
        if (!it) {
            std::cout << "all blocks are used" << std::endl;
            return 1;
        }
        it.alloc(data_size);
        std::memcpy(it->data(), arg.c_str(), data_size);
        std::cout << "succesfully wrote: \"" << arg << "\"" << std::endl;
    }
    log_tl(tl);
    return 0;
}

int handle_remove(ts::TupleList &tl, const std::string &arg) {
    {
        auto it = tl.iter();
        while (it && (it->is_free() || std::string(it->data()) != arg))
            ++it;
        if (!it) {
            std::cout << "not found: \"" << arg << "\"" << std::endl;
            return 1;
        }
        it.free();
        std::cout << "succesfully removed: \"" << arg << "\"" << std::endl;
    }
    log_tl(tl);
    return 0;
}

int handle_print(ts::TupleList &tl) {
    {
        bool any_found = false;
        for (auto it = tl.iter(); it; ++it) {
            if (it->is_free())
                continue;
            std::string value(it->data());
            std::cout << value << std::endl;
            any_found = true;
        }
        if (!any_found)
            std::cout << "list is empty" << std::endl;
    }
    log_tl(tl);
    return 0;
}

static int handle_enqueue(ts::NewTupleQueue &q, const std::string &arg) {
    uint32_t value = atoi(arg.c_str());
    q.add(value);
    std::cout << "enqueued: " << value << std::endl;
    return 0;
}

static int handle_dequeue(ts::NewTupleQueue &q) {
    std::cout << "dequeued: " << q.take() << std::endl;
    return 0;
}
