//
// Created by imat on 21.12.23.
//

#ifndef TUPLESPACE_SHELL_H
#define TUPLESPACE_SHELL_H

#include "AbstractTuplespaceAPI.h"
#include "Tuple.h"
#include "TuplePattern.h"
#include <iostream>
#include <stdexcept>
#include "Utils.h"

class Shell {
public:
    Shell(AbstractTuplespaceApi *tuplespaceApi, std::istream &commandStream,
          std::ostream &outputStream, bool raw = false)
        : tuplespaceApi(tuplespaceApi), commandStream(commandStream),
          outputStream(outputStream), rawOutput(raw){};
    void mainLoop();

    void registerSignalHandlers();
private:
    AbstractTuplespaceApi *tuplespaceApi;
    bool working = true;
    std::istream &commandStream;
    std::ostream &outputStream;
    bool rawOutput;

    void handleFunction(const std::string &function, const std::string &args);
    void handleTSinput(const std::string &args);
    void handleTSoutput(const std::string &args);
    void handleTSread(const std::string &args);
    void availableFuncs();
};

#endif // TUPLESPACE_SHELL_H
