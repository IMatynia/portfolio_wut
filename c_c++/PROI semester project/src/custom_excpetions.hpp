#ifndef __CUSTOM_EXCPETIONS_H__
#define __CUSTOM_EXCPETIONS_H__

#include <stdexcept>
#include <string>

using std::string;

class BoothIsEmptyError : public std::logic_error
{
public:
    BoothIsEmptyError(uint id) : std::logic_error("Booth of id " + std::to_string(id) + " is empty!") {}
};

class BoothIsNotEmptyError : public std::logic_error
{
public:
    BoothIsNotEmptyError(uint id) : std::logic_error("Booth of id " + std::to_string(id) + " is not empty!") {}
};

class InvalidMusicGenreError : public std::runtime_error
{
public:
    InvalidMusicGenreError(int id) : std::runtime_error("Id " + std::to_string(id) + " does not point to any known genres"){};
};

class WrongStateOrderError : public std::logic_error
{
public:
    WrongStateOrderError(uint customer_id, string new_state, string old_state) : std::logic_error("Customer with id=" + std::to_string(customer_id) + " cannot go into " + new_state + " right now because its state is " + old_state){};
};

class BadInputDataError : public std::runtime_error
{
public:
    BadInputDataError(std::string whereabouts) : std::runtime_error("Bad data was given as input for " + whereabouts){};
};

class EmptyDatabaseError : public std::logic_error
{
public:
    EmptyDatabaseError() : std::logic_error("Current vinyl database is empty"){};
};

class FileNotGoodError : public std::runtime_error
{
public:
    FileNotGoodError(std::string filename) : std::runtime_error("The was a problem opening file " + filename){};
};

class ComandLineArgumentError : public std::runtime_error
{
    const string HELP = "The program needs the following arguments to run properly:\n"
                        "$main SIMULATION_TIME N_OF_BOOTHS DISC_JSON CUSTOMER_JSON CONFIG_JSON [LOG_FILE]\n"
                        "- **SIMULATION_TIME** - Number of ticks to run the simulation for\n"
                        "- **N_OF_BOOTHS** - Number of booths available in the shop for the customers to use\n"
                        "- **DISC_JSON** - A json file containing disc database information, detailed description of file structiure is in VinylDatabase::VinylDatabase\n"
                        "- **CUSTOMER_JSON** - A json file with the descriptions of all customers and their actions. Detailed description of the file structure in Customer::Customer\n"
                        "- **LOG_FILE** - Path to a file where the log will be saved\n"
                        "- **[DELAY_MS]** - Delay between simulation ticks\n"
                        "You can also run the program with -h to view this information\n";

public:
    ComandLineArgumentError(std::string detail) : std::runtime_error(detail + "\n\n" + HELP){};
};

class JsonMissingFieldError : public std::runtime_error
{
public:
    JsonMissingFieldError(std::string what_file, std::string details) : std::runtime_error("Error reading data from " + what_file + "\n\n" + details){};
};

class NonPositiveValueError : public std::logic_error
{
public:
    NonPositiveValueError(string variable) : std::logic_error(variable + " must be positive."){};
};

#endif // __CUSTOM_EXCPETIONS_H__