//
// Created by imat on 21.12.23.
//

#ifndef TUPLESPACE_DUMMYTUPLESPACE_H
#define TUPLESPACE_DUMMYTUPLESPACE_H

#include "AbstractTuplespaceAPI.h"
#include <iostream>

class DummyTuplespace : public AbstractTuplespaceApi {
public:
    void init() override { std::cout << "tuplespace initialized!\n"; };

    void TSoutput(const Tuple &newTuple) override {
        (void)newTuple;
        std::cout << "tuplespace new tuple!\n";
    };

    Tuple *TSinput(const TuplePattern &tuplePattern, int timeoutMs) override {
        (void)tuplePattern;
        (void)timeoutMs;
        std::cout << "tuplespace input!\n";
        return &fake_tuple;
    };

    Tuple *TSread(const TuplePattern &tuplePattern, int timeoutMs) override {
        (void)tuplePattern;
        (void)timeoutMs;
        std::cout << "tuplespace read!\n";
        return &fake_tuple;
    };

    void close() override { std::cout << "tuplespace closed!\n"; };

    ~DummyTuplespace() override = default;

private:
    std::string fake_repr = "\"hello\", 15, 2.5";
    std::string fake_repr_pattern = "integer:15, float:>2.5";
    Tuple fake_tuple = Tuple(fake_repr);
    TuplePattern fake_pattern = TuplePattern(fake_repr_pattern);
};

#endif // TUPLESPACE_DUMMYTUPLESPACE_H
