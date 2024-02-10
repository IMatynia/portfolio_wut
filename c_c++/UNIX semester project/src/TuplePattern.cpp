//
// Created by imat on 21.12.23. Changed by vshpakov 11.01.24
//
#include "TuplePattern.h"

std::map<std::string, ComparisonOperator> OperatorAssignment = {
    {"==", ComparisonOperator::EQUAL},
    {">=", ComparisonOperator::GREATER_THAN_OR_EQUAL},
    {"<=", ComparisonOperator::LESS_THAN_OR_EQUAL},
    {">", ComparisonOperator::GREATER_THAN},
    {"<", ComparisonOperator::LESS_THAN},
    {"*", ComparisonOperator::ANY},
};

std::map<ComparisonOperator, std::string> OperatorString = {
    {ComparisonOperator::EQUAL, "=="},
    {ComparisonOperator::GREATER_THAN_OR_EQUAL, ">="},
    {ComparisonOperator::LESS_THAN_OR_EQUAL, "<="},
    {ComparisonOperator::GREATER_THAN, ">"},
    {ComparisonOperator::LESS_THAN, "<"},
    {ComparisonOperator::ANY, "*"}};

PatternField TuplePattern::parseField(std::string &fieldString) {
    PatternField field;
    size_t pos = fieldString.find(':');
    if (pos == std::string::npos)
        throw std::invalid_argument("Malformed tuple field");

    std::string typeString = fieldString.substr(0, pos);
    std::string conditionString = fieldString.substr(pos + 1);

    return parseCondition(typeString, conditionString);
};

TuplePattern::TuplePattern(std::string textRepresentation) {
    textRepresentation.erase(std::remove_if(textRepresentation.begin(),
                                            textRepresentation.end(),
                                            ::isspace),
                             textRepresentation.end());

    const char fieldDelimiter = ',';

    std::istringstream fieldStream(textRepresentation);
    std::string fieldStr;
    while (std::getline(fieldStream, fieldStr, fieldDelimiter)) {
        PatternField patternField = parseField(fieldStr);
        fields.push_back(patternField);
    }
};

bool TuplePattern::compareSlow(const Tuple &tuple) const {
    if (fields.size() != tuple.fields.size()) {
        return false; // Number of fields doesn't match
    }

    for (size_t i = 0; i < fields.size(); ++i) {
        const PatternField &patternField = fields[i];
        const auto &tupleValue = tuple.getFieldAtIndex(i);
        if (!isTypeMatch(tupleValue, patternField.data)) {
            return false; // Types don't match
        }
        // Perform comparison based on the operator
        switch (patternField.op) {
        case ComparisonOperator::LESS_THAN:
            if (tupleValue >= patternField.data) {
                return false;
            };
            break;
        case ComparisonOperator::LESS_THAN_OR_EQUAL:
            if (tupleValue > patternField.data) {
                return false;
            };
            break;
        case ComparisonOperator::EQUAL:
            if (tupleValue != patternField.data) {
                return false;
            };
            break;
        case ComparisonOperator::GREATER_THAN:
            if (tupleValue <= patternField.data) {
                return false;
            };
            break;
        case ComparisonOperator::GREATER_THAN_OR_EQUAL:
            if (tupleValue < patternField.data) {
                return false;
            };
            break;
        case ComparisonOperator::ANY:
            break;
        }
    }

    // All fields matched
    return true;
}

std::string TuplePattern::toString() {
    std::string result;

    for (const auto &field : fields) {
        // Convert data type to string
        std::string typeStr;
        if (std::holds_alternative<int64_t>(field.data)) {
            typeStr = "integer";
        } else if (std::holds_alternative<float>(field.data)) {
            typeStr = "float";
        } else if (std::holds_alternative<std::string>(field.data)) {
            typeStr = "string";
        }

        // Convert operator to string
        std::string opStr = OperatorString[field.op];

        // Convert data value to string
        std::string valueStr;
        if (std::holds_alternative<int64_t>(field.data)) {
            valueStr = std::to_string(std::get<int64_t>(field.data));
        } else if (std::holds_alternative<float>(field.data)) {
            valueStr = std::to_string(std::get<float>(field.data));
        } else if (std::holds_alternative<std::string>(field.data)) {
            valueStr = std::get<std::string>(field.data);
        }

        bool isStarOperator = field.op == ComparisonOperator::ANY;

        result +=
            typeStr + ":" + opStr + (isStarOperator ? "" : valueStr) + ",";
    }

    // Remove the trailing comma if there are fields
    if (!fields.empty()) {
        result.pop_back();
    }

    return result;
}

PatternField TuplePattern::parseCondition(std::string &typeString,
                                          std::string conditionString) {
    ComparisonOperator op = deduceOperator(conditionString);
    VariantType value;

    if (typeString == "integer") {
        if (op == ComparisonOperator::ANY)
            value = std::int64_t(0);
        else
            value = std::stoll(conditionString);
    } else if (typeString == "float") {
        if (op == ComparisonOperator::ANY)
            value = 0.0f;
        else
            value = std::stof(conditionString);
    } else if (typeString == "string") {
        if (op == ComparisonOperator::ANY)
            value = std::string("");
        else {
            if (!checkValidString(conditionString))
                throw std::invalid_argument(
                    "Strings need to have quotation marks");
            value = conditionString.substr(1, conditionString.size() - 2);
        }
    } else {
        throw std::invalid_argument("Invalid field type in tuple pattern");
    }

    checkConditionAllowed(op, value);
    return {.data = value, .op = op};
}

ComparisonOperator TuplePattern::deduceOperator(std::string &conditionString) {
#define MAX_OPERATOR_LENGTH 2
    for (int i = MAX_OPERATOR_LENGTH; i > 0; --i) {
        std::string comp = conditionString.substr(0, i);
        auto it = OperatorAssignment.find(comp);
        if (it != OperatorAssignment.end()) {
            conditionString =
                conditionString.substr(it->first.size(), std::string::npos);
            return it->second;
        }
    }
    return ComparisonOperator::EQUAL;
}

void TuplePattern::checkConditionAllowed(ComparisonOperator op,
                                         VariantType value) {
    if (op == ComparisonOperator::EQUAL &&
        std::holds_alternative<float>(value)) {
        throw std::invalid_argument("Float can't use equals operator!");
    }
}

const std::vector<PatternField> &TuplePattern::getFields() const {
    return fields;
}

bool TuplePattern::checkValidString(std::string basicString) {
    return *basicString.begin() == '"' && *(basicString.end() - 1) == '"';
}
