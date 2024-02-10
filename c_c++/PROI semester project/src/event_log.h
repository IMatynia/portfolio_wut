#ifndef __EVENT_LOG_H__
#define __EVENT_LOG_H__

#include "event.h"
#include <vector>

/// Stores and prints to streams events that happen in the shop
class EventLog
{
private:
    bool single_ostream;
    std::vector<Event> history;
    std::ostream &terminal;
    std::ostream &filestream;

public:
    /**
     * @brief Construct a new Event Log object. Filestream is optional.
     *
     * @param new_terminal      Primary (terminal) ostream
     * @param new_filestream    Additional (file) ostream (optional)
     */
    EventLog(std::ostream &new_terminal, std::ostream &new_filestream);
    EventLog(std::ostream &new_terminal);
    /// Adds the event to the log
    void addEvent(const Event &new_event);
};

#endif // __EVENT_LOG_H__