//
// Created by imat on 21.12.23.
//

#ifndef TUPLESPACE_ABSTRACTTUPLESPACEAPI_H
#define TUPLESPACE_ABSTRACTTUPLESPACEAPI_H
#include "Tuple.h"
#include "TuplePattern.h"

class AbstractTuplespaceApi {
public:
    virtual void init() = 0;
    virtual void TSoutput(const Tuple &newTuple) = 0;
    virtual Tuple *TSinput(const TuplePattern &tuplePattern, int timeoutMs) = 0;
    virtual Tuple *TSread(const TuplePattern &tuplePattern, int timeoutMs) = 0;
    virtual void close() = 0;
    virtual ~AbstractTuplespaceApi() = default;
};

#endif // TUPLESPACE_ABSTRACTTUPLESPACEAPI_H
