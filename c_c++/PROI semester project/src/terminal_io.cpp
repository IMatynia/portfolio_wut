#include "terminal_io.h"

TerminalIO::TerminalIO(std::istream &command_stream, std::ostream &output_stream, VinylRecordShop &shop_new) : output(output_stream), cmd(command_stream), shop(shop_new), next_breakpoint(0)
{
}

void TerminalIO::query(uint tick)
{
    output << "[!] Timestamp == " << tick << std::endl;

    std::string command = "";
    std::string subcommand = "";
    while (!(command == "continue" || command == "cont") && !cmd.eof() && cmd.good())
    {
        command = "q";
        output << "[$] ";
        cmd >> command;

        try
        {
            if (command == "status" || command == "stat")
            {
                output << shop << std::endl;
            }
            else if (command == "booth" || command == "bth")
            {
                cmd >> subcommand;
                boothCommand(subcommand);
            }
            else if (command == "queue" || command == "qu")
            {
                cmd >> subcommand;
                queueCommand(subcommand);
            }
            else if (command == "database" || command == "db")
            {
                shop.printDatabase(output);
            }
            else if (command == "break" || command == "br")
            {
                uint timestamp;
                cmd >> timestamp;
                setBreakpoint(timestamp);
            }
            else if (command == "continue" || command == "cont")
            {
                continue;
            }
            else if (command == "run")
            {
                setBreakpoint(UINT32_MAX);
                break;
            }
            else if (command == "exit" || command == "q")
            {
                throw TerminalIOExitSignal();
            }
            else
            {
                output << "[!] Available commands are:\n"
                          "> status (stat)                  -> shop status\n"
                          "> booth (bth)      [SUBCOMMAND] [PARAMETERS]\n"
                          "> queue (qu)       [SUBCOMMAND] [PARAMETERS]\n"
                          "> database (db)                  -> display database info\n"
                          "> break (br)       TIMESTAMP     -> stop simulation at timestamp\n"
                          "> continue (cont)                -> continiue untill the next breakpoint or step 1 frame\n"
                          "> run                            -> run til completion\n"
                          "> exit (q)\n";
            }
        }
        catch (const TerminalIOExitSignal &e)
        {
            throw;
        }
        catch (const std::exception &e)
        {
            cmd.clear();
            output << "[!] ERROR!\n"
                   << e.what() << std::endl;
        }
    }
}

bool TerminalIO::isBreakpoint(uint tick) const
{
    return tick >= next_breakpoint;
}

void TerminalIO::setBreakpoint(uint tick)
{
    next_breakpoint = tick;
}

void TerminalIO::boothCommand(std::string subcommand)
{
    size_t index;
    if (subcommand == "list" || subcommand == "ls")
    {
        shop.printBooths(output);
    }
    else if (subcommand == "print" || subcommand == "p")
    {
        cmd >> index;
        if (cmd.fail())
            throw BadInputDataError("index number (uint)");
        shop.getBoothRef(index)->fullSummary(output);
    }
    else
    {
        output << "[!] Available subcommands for booth are:\n"
                  "> list (ls)              -> prints all booths\n"
                  "> print (p)      [INDEX] -> prints detailed info of a booth\n";
    }
}

void TerminalIO::queueCommand(std::string subcommand)
{
    size_t index;
    if (subcommand == "list" || subcommand == "ls")
    {
        shop.printQueue(output);
    }
    else if (subcommand == "print" || subcommand == "p")
    {
        cmd >> index;
        if (cmd.fail())
            throw BadInputDataError("index number (uint)");
        shop.getCustomerRef(index)->fullSummary(output);
    }
    else
    {
        output << "[!] Available subcommands for queue are:\n"
                  "> list (ls)              -> prints all customers in queue\n"
                  "> print (p)      [INDEX] -> prints detailed info of a customer\n";
    }
}