#include <string>
#include <iostream>
#include "person.h"

Person::Person(std::string first_name, std::string last_name) : first_name(first_name), last_name(last_name) {}
Person::Person() {}

void Person::set_first_name(std::string new_name)
{
    first_name = new_name;
}

void Person::set_last_name(std::string new_name)
{
    last_name = new_name;
}

std::string Person::get_first_name() const
{
    return first_name;
}

std::string Person::get_last_name() const
{
    return last_name;
}

std::string Person::get_full_name() const
{

    if (first_name == "" && last_name == "")
    {
        return "Unkown";
    }
    else
    {
        return first_name + " " + last_name;
    }
}

Person::~Person() {}
