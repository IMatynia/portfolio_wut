//
// Created by imat on 21.12.23. Changed by vshpakov 08.01.24
//

#include "Tuple.h"

Tuple::Tuple(std::string textRepresentation) {
    parse(textRepresentation);
    isReadByWaitingProcesses = false;
};

Tuple::~Tuple(){};

void Tuple::addField(const std::string &fieldAsStr) {
    // String
    if (*fieldAsStr.begin() == '"' && *(fieldAsStr.end() - 1) == '"') {
        fields.emplace_back(fieldAsStr.substr(1, fieldAsStr.size() - 2));
        return;
    }
    // Non-strings:
    // Float?
    auto dotPos = fieldAsStr.find(".");
    if (dotPos == std::string::npos) {
        // Integer
        fields.emplace_back(std::stoll(fieldAsStr));
        return;
    } else {
        // Float
        fields.emplace_back(std::stof(fieldAsStr));
        return;
    }
};

std::string Tuple::toString() {
    std::string result = "(";
    for (const auto &field : fields) {
        if (auto intValue = std::get_if<std::int32_t>(&field)) {
            result += std::to_string(*intValue) + ", ";
        } else if (auto longValue = std::get_if<std::int64_t>(&field)) {
            result += std::to_string(*longValue) + ", ";
        } else if (auto strValue = std::get_if<std::string>(&field)) {
            result += *strValue + ", ";
        } else if (auto floatValue = std::get_if<float>(&field)) {
            result += std::to_string(*floatValue) + ", ";
        } else if (auto stringValue = std::get_if<std::string>(&field)) {
            result += *stringValue + ", ";
        }
    }
    // remove the trailing comma and space
    if (!fields.empty()) {
        result.pop_back();
        result.pop_back();
    }
    result += ")";
    return result;
};

void Tuple::parse(std::string textRepresentation) {

    textRepresentation.erase(std::remove_if(textRepresentation.begin(),
                                            textRepresentation.end(),
                                            ::isspace),
                             textRepresentation.end());

    const char fieldDelimiter = ',';

    std::istringstream fieldStream(textRepresentation);
    std::string fieldStr;
    while (std::getline(fieldStream, fieldStr, fieldDelimiter)) {
        addField(fieldStr);
    }
};

size_t Tuple::serialize(std::string &buffer) const {
    buffer.clear();
    size_t originalSize = buffer.size();
    // Tuple Header
    // waiting flag
    buffer.push_back(isReadByWaitingProcesses ? 1 : 0);

    // fields number
    uint32_t numberOfFields = static_cast<uint32_t>(fields.size());
    buffer.insert(
        buffer.end(), reinterpret_cast<const uint8_t *>(&numberOfFields),
        reinterpret_cast<const uint8_t *>(&numberOfFields) + sizeof(uint32_t));

    // field types
    for (const auto &field : fields) {
        if (std::holds_alternative<std::int32_t>(field)) {
            buffer.push_back(0b00);
        } else if (std::holds_alternative<std::int64_t>(field)) {
            buffer.push_back(0b01);
        } else if (std::holds_alternative<float>(field)) {
            buffer.push_back(0b10);
        } else if (std::holds_alternative<std::string>(field)) {
            buffer.push_back(0b11);
        }
    }
    // Tuple data
    for (const auto &field : fields) {
        if (auto intValue = std::get_if<std::int32_t>(&field)) {
            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t *>(intValue),
                          reinterpret_cast<const uint8_t *>(intValue) +
                              sizeof(std::int32_t));
        } else if (auto longValue = std::get_if<std::int64_t>(&field)) {
            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t *>(longValue),
                          reinterpret_cast<const uint8_t *>(longValue) +
                              sizeof(std::int64_t));
        } else if (auto floatValue = std::get_if<float>(&field)) {
            buffer.insert(
                buffer.end(), reinterpret_cast<const uint8_t *>(floatValue),
                reinterpret_cast<const uint8_t *>(floatValue) + sizeof(float));
        } else if (auto stringValue = std::get_if<std::string>(&field)) {
            uint16_t strLength = static_cast<uint16_t>(stringValue->length());
            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t *>(&strLength),
                          reinterpret_cast<const uint8_t *>(&strLength) +
                              sizeof(uint16_t));
            buffer.insert(buffer.end(), stringValue->begin(),
                          stringValue->end());
        }
    }
    return buffer.size() - originalSize;
};

Tuple Tuple::deserialize(const std::string &buffer) {
    Tuple result;
    size_t index = 0;

    // waiting flag
    result.isReadByWaitingProcesses = (buffer[index++] == 1);

    // number of fields
    uint32_t numFields;
    std::memcpy(&numFields, &buffer[index], sizeof(uint32_t));
    index += sizeof(uint32_t);

    // separate field types and field data
    std::vector<uint8_t> fieldTypes(buffer.begin() + index,
                                    buffer.begin() + index + numFields);
    index += numFields;

    // process fields
    for (size_t i = 0; i < numFields; ++i) {
        uint8_t fieldType = fieldTypes[i];

        switch (fieldType) {
        case 0b00: {
            std::int32_t intValue;
            std::memcpy(&intValue, &buffer[index], sizeof(std::int32_t));
            result.fields.push_back(intValue);
            index += sizeof(std::int32_t);
            break;
        }
        case 0b01: {
            std::int64_t longValue;
            std::memcpy(&longValue, &buffer[index], sizeof(std::int64_t));
            result.fields.push_back(longValue);
            index += sizeof(std::int64_t);
            break;
        }
        case 0b10: {
            float floatValue;
            std::memcpy(&floatValue, &buffer[index], sizeof(float));
            result.fields.push_back(floatValue);
            index += sizeof(float);
            break;
        }
        case 0b11: {
            uint16_t strLength;
            std::memcpy(&strLength, &buffer[index], sizeof(uint16_t));
            index += sizeof(uint16_t);

            std::string stringValue(buffer.begin() + index,
                                    buffer.begin() + index + strLength);
            result.fields.push_back(stringValue);
            index += strLength;
            break;
        }
        default:
            throw std::runtime_error(
                "Unknown field type during deserialization");
        }
    }

    return result;
}

const std::vector<VariantType> &Tuple::getFields() const { return fields; }

bool Tuple::operator==(const Tuple &other) const {
    auto otherFields = other.getFields();
    if (otherFields.size() != fields.size())
        return false;

    for (size_t i = 0; i < fields.size(); ++i) {
        if (otherFields[i] != fields[i]) {
            return false;
        }
    }
    return true;
}

bool Tuple::operator!=(const Tuple &other) const { return !(*this == other); }
