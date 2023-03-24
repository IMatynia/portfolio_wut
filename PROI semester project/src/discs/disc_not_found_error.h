#ifndef DISC_NOT_FOUND_ERROR_H
#define DISC_NOT_FOUND_ERROR_H

#include<stdexcept>

class DiscNotFoundError: public std::invalid_argument
{
    public:
        DiscNotFoundError(std::string name);
};

#endif