#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <string>
#include <iostream>
#include <chrono>
#include <random>
#include "discs/disc.h"

/**
 * @brief Takes the input stream as long as it is available and then returns it as a string
 *
 * @param is            Input stream
 * @return std::string  A string of all text from the stream
 */
std::string stream_to_string(std::istream &is);
uint randint(uint max_value);
uint randint_range(uint range_start, uint range_stop);
DiscAction get_random_disc_action();
#endif // __UTILITIES_H__