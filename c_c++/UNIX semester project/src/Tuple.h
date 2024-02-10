//
// Created by imat on 21.12.23. Changed by vshpakov 08.01.24
//

#ifndef TUPLESPACE_TUPLE_H
#define TUPLESPACE_TUPLE_H

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <typeindex>
#include <variant>
#include <vector>

#include "TupleShared.h"

class Tuple {
public:
    Tuple() : isReadByWaitingProcesses(false){};
    Tuple(std::string textRepresentation);
    ~Tuple();
    void addField(const std::string &fieldAsStr);
    size_t serialize(std::string &buffer) const;
    static Tuple deserialize(const std::string &buffer);
    std::string toString();
    bool isReadByWaitingProcesses;
    std::vector<VariantType> fields;

    const std::vector<VariantType> &getFields() const;

    const VariantType &getFieldAtIndex(size_t index) const {
        return fields.at(index);
    }
    bool operator==(const Tuple &other) const;
    bool operator!=(const Tuple &other) const;

private:
    void parse(std::string textRepresentation);
};

#endif // TUPLESPACE_TUPLE_H
