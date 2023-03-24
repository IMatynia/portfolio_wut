#include <string>
#include <iostream>
#include "author.h"

Author::Author(json &json_obj)
{
    first_name = json_obj["name"];
    last_name = json_obj["surname"];
}

Author::Author(std::string first_name, std::string last_name): Person(first_name, last_name){}


std::ostream &operator<<(std::ostream &os, const Author &author)
{
    if (author.get_full_name() != " ")
    {
        os << author.get_full_name();
    }
    else
    {
        os << "Unkown";
    }
    return os;
}