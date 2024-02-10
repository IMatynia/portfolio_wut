#ifndef __TERMINAL_IO_H__
#define __TERMINAL_IO_H__

#include <iostream>
#include "vinyl_record_shop.h"

/**
 * @brief Handles IO of commands to work on the shop
 *
 */
class TerminalIO
{
    std::istream &cmd;
    std::ostream &output;
    VinylRecordShop &shop;

    uint next_breakpoint;

public:
    /**
     * @brief Construct a new Terminal I O object
     *
     * @param command_stream    stream with input commands
     * @param output_stream     stream for output
     * @param shop_new          the shop which will be investigated
     */
    TerminalIO(std::istream &command_stream, std::ostream &output_stream, VinylRecordShop &shop_new);
    /// Begins command interpretation
    void query(uint tick);
    /// Checks if the world loop should stop for the next query
    bool isBreakpoint(uint tick) const;
    /// Sets where at what tick should the next breakpoint be
    void setBreakpoint(uint tick);
    /// Handles all commands related to booths
    void boothCommand(std::string subcommand);
    /// Handles all commands related to the queue
    void queueCommand(std::string subcommand);
};

class TerminalIOExitSignal : public std::exception
{
public: TerminalIOExitSignal() : std::exception(){};
};

#endif // __TERMINAL_IO_H__