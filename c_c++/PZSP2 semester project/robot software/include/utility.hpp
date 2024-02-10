#ifndef INCLUDE_UTIL_HPP_
#define INCLUDE_UTIL_HPP_

#define M_PI (3.14159265358979323846)
#define PRECISION_1 10
#define PRECISION_2 100
#define PRECISION_3 1000
#define PRECISION_4 10000

#include <string>
inline int floatingpoint_to_int(float x, int precision)
{
    return (int)(x*precision);
}

inline int floatingpoint_to_int(double x, int precision)
{
    return (int)(x*precision);
}

inline long floatingpoint_to_long(float x, int precision)
{
    return (long)(x*precision);
}

inline long floatingpoint_to_long(double x, int precision)
{
    return (long)(x*precision);
}
#endif

