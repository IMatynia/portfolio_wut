#include "event_log.h"
EventLog::EventLog(std::ostream &new_terminal, std::ostream &new_filestream) : terminal(new_terminal), filestream(new_filestream)
{
    single_ostream = false;
}

EventLog::EventLog(std::ostream &new_terminal) : terminal(new_terminal), filestream(new_terminal)
{
    single_ostream = true;
}

void EventLog::addEvent(const Event &new_event)
{
    history.push_back(new_event);
    terminal << new_event << std::endl;
    if (!single_ostream)
        filestream << new_event << std::endl;
}
