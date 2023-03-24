#include "event.h"
uint Event::next_id = 0;

Event::Event(int new_timestamp) : timestamp(new_timestamp), id(next_id++)
{
}

int Event::getTimestamp() const
{
    return timestamp;
}

std::ostream &Event::printToStream(std::ostream &os) const
{
    os << "[T=" << std::setw(7) << timestamp << "] => ";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Event &event)
{
    return event.printToStream(os);
}
