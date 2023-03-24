#ifndef EMPTY_TITLE_ERROR_H
#define EMPTY_TITLE_ERROR_H

#include<stdexcept>

class EmptyTitleError: public std::invalid_argument
{
    public:
        EmptyTitleError();
};

#endif