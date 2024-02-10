#ifndef NEGATIVE_PRICE_ERROR_H
#define NEGATVIE_PRICE_ERROR_H

#include<stdexcept>

class NegavitePriceError: public std::invalid_argument
{
    public:
        NegavitePriceError();
};

#endif