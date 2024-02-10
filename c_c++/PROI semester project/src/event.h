#ifndef __EVENT_H__
#define __EVENT_H__

#include <iostream>
#include <iomanip>

/// Represents a single event in an event log. Base class.
class Event
{
private:
    static uint next_id;
    const uint id;

protected:
    const int timestamp;

public:
    /**
     * @brief Construct a blank Event object. This is a base class so the event only has a header with the time stamp
     *
     * @param new_timestamp     Time of the event
     */
    Event(int new_timestamp);
    virtual ~Event(){};

    int getTimestamp() const;
    /// Prints contents of the event onto a stream
    virtual std::ostream &printToStream(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const Event &event);
};

std::ostream &operator<<(std::ostream &os, const Event &event);

#endif // __EVENT_H__