#include "utilities.h"
// #include "discs/disc.h"

std::string stream_to_string(std::istream &is)
{
    std::string out;
    std::string temp;
    while (is.good() && !is.eof())
    {
        std::getline(is, temp);
        out += (temp + "\n");
    }
    return out;
}

uint randint(uint max_value)
{
    if (max_value == 0)
    {
        return 0;
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    return generator() % (max_value + 1);
}

uint randint_range(uint range_start, uint range_stop)
{
    if (range_start > range_stop)
        std::swap(range_start, range_stop);
    return randint(range_stop - range_start) + range_start;
}

DiscAction get_random_disc_action()
{
    int action = randint(2);
    DiscAction disc_action = (DiscAction) action;
    return disc_action;

}