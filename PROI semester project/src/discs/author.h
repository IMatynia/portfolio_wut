#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include "../json.hpp"
#include "../person.h"

using nlohmann::json;

/// Class represening author of the disc
class Author: public Person
{
public:
    Author() = default;
    Author(std::string first_name, std::string last_name);
    Author(json &json_obj);

    friend std::ostream &operator<<(std::ostream os, const Author &author);
};

std::ostream &operator<<(std::ostream os, const Author &author);

#endif