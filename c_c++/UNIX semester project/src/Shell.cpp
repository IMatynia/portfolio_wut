//
// Created by imat on 21.12.23.
//

#include "Shell.h"

void Shell::mainLoop() {
    std::string fullCommand, function, args;
    working = true;
    if (!rawOutput)
        outputStream << "Type exit to quit shell" << std::endl;
    while (working && !commandStream.eof() && !quit) {
        if (!rawOutput)
            outputStream << " > ";
        getline(commandStream, fullCommand);
        size_t separator_position = fullCommand.find(' ');

        function = fullCommand.substr(0, separator_position);
        args = fullCommand.substr(separator_position + 1);

        if (function == "exit" || function.empty()) {
            working = false;
        } else {
            try {
                this->handleFunction(function, args);
            } catch (std::system_error &e) {
                outputStream << "ERROR: " << e.what() << std::endl;
                perror(e.what());
                exit(EXIT_FAILURE);
            } catch (std::exception &e) {
                outputStream << "ERROR: " << e.what() << std::endl;
            }
        }
    }
}

void Shell::handleFunction(const std::string &function,
                           const std::string &args) {
    if (function == "TSinput") {
        handleTSinput(args);
    } else if (function == "TSoutput") {
        handleTSoutput(args);
    } else if (function == "TSread") {
        handleTSread(args);
    } else {
        outputStream << "ERROR: " << function << " is not a valid function!"
                     << std::endl;
        availableFuncs();
    }
}

void Shell::handleTSread(const std::string &args) {
    size_t lastSpacePos = args.find_last_of(' ');
    if (lastSpacePos == std::string::npos) {
        outputStream << "ERROR: No timeout set!" << std::endl;
        return;
    }
    int timeoutMs = std::stoi(args.substr(lastSpacePos + 1));
    TuplePattern pattern(args.substr(0, lastSpacePos));
    Tuple *found = tuplespaceApi->TSread(pattern, timeoutMs);
    if (found == nullptr)
        outputStream << "ERROR: TSread timed out, tuple was not found!"
                     << std::endl;
    else
        outputStream << found->toString() << std::endl;
    delete found;
}

void Shell::handleTSoutput(const std::string &args) {
    Tuple newTuple(args);
    tuplespaceApi->TSoutput(newTuple);
}

void Shell::handleTSinput(const std::string &args) {
    size_t lastSpacePos = args.find_last_of(' ');
    if (lastSpacePos == std::string::npos) {
        outputStream << "ERROR: No timeout set!" << std::endl;
        return;
    }
    int timeoutMs = std::stoi(args.substr(lastSpacePos + 1));

    TuplePattern pattern(args.substr(0, lastSpacePos));
    Tuple *found = tuplespaceApi->TSinput(pattern, timeoutMs);

    if (found == nullptr)
        outputStream << "ERROR: TSinput timed out, tuple was not found!"
                     << std::endl;
    else
        outputStream << found->toString() << std::endl;
    delete found;
}

void Shell::availableFuncs() {
    outputStream << "Available functions are:\n"
                    "TSoutput <tuple>\n"
                    "TSinput <tuple pattern> <timeout>\n"
                    "TSread <tuple pattern> <timeout>\n";
}

void Shell::registerSignalHandlers() {
    struct sigaction sa{};
    sa.sa_handler = quit_signal_handler;
    sa.sa_flags = 0;
    sigfillset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
}
