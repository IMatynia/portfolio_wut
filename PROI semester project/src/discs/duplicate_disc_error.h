#ifndef DUPLICATE_DISC_ERROR_H
#define DUPLICATE_DISC_ERROR_H

#include<stdexcept>
#include<string>

class DuplicateDiscError: public std::invalid_argument
{
    public:
        DuplicateDiscError(std::string genre, std::string name);
};

#endif