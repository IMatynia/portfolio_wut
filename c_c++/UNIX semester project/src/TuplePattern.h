//
// Created by imat on 21.12.23. Changed by vshpakov 11.01.24
//

#ifndef TUPLESPACE_TUPLEPATTERN_H
#define TUPLESPACE_TUPLEPATTERN_H

#include "Tuple.h"
#include "TupleShared.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <typeindex>
#include <variant>
#include <vector>

enum class ComparisonOperator {
    EQUAL,
    LESS_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL,
    ANY
};

extern std::map<std::string, ComparisonOperator> OperatorAssignment;
extern std::map<ComparisonOperator, std::string> OperatorString;

struct PatternField {
    VariantType data;
    ComparisonOperator op;
};

class TuplePattern {
public:
    explicit TuplePattern(std::string textRepresentation);

    bool compareSlow(const Tuple &tuple) const;

    std::string toString();

    bool isTypeMatch(const VariantType &a, const VariantType &b) const {
        return a.index() == b.index();
    }

    const std::vector<PatternField> &getFields() const;

private:
    std::vector<PatternField> fields;

    PatternField parseField(std::string &fieldString);

    static PatternField parseCondition(std::string &typeString,
                                       std::string conditionString);

    static ComparisonOperator deduceOperator(std::string &conditionString);

    static void checkConditionAllowed(ComparisonOperator op, VariantType value);

    static bool checkValidString(std::string basicString);
};

#endif // TUPLESPACE_TUPLEPATTERN_H
