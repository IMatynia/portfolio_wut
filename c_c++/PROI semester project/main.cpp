#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "src/vinyl_record_shop.h"
#include "src/utilities.h"
#include "src/terminal_io.h"

#define SLEEP_TIME_DEFAULT 50
#define NO_DELAY_AC 6
#define DELAY_AC 7
#define SIGN_BIT 1<<31

using std::cin;
using std::cout;

/**
    # Running the shop simulation

    ## Comand line arguments

    The programs need the following arguments to run properly:

    ```sh
    $main SIMULATION_TIME N_OF_BOOTHS DISC_JSON CUSTOMER_JSON LOG_FILE [DELAY_MS]
    ```

    - **SIMULATION_TIME** - Number of ticks to run the simulation for
    - **N_OF_BOOTHS** - Number of booths available in the shop for the customers to use
    - **DISC_JSON** - A json file containing disc database information, detailed description of file structiure is in VinylDatabase::VinylDatabase
    - **CUSTOMER_JSON** - A json file with the descriptions of all customers and their actions. Detailed description of the file structure in Customer::Customer
    - **LOG_FILE** - Path to a file where the log will be saved
    - **[DELAY_MS]** - Delay between simulation ticks

    You can also run the program with -h to view this information
 */

int main(int ac, char *av[])
{
    string log_file, customer_file, discs_file;
    uint sim_time, n_of_booths;
    uint delay = SLEEP_TIME_DEFAULT;

    try
    {
        std::stringstream arguments;
        if (ac < NO_DELAY_AC || ac > DELAY_AC)
        {
            throw ComandLineArgumentError("Invalid argument count");
        }

        for (size_t i = 1; i < ac; i++)
        {
            arguments << av[i] << std::endl;
        }
        arguments >> sim_time >> n_of_booths >> discs_file >> customer_file >> log_file;
        if (ac == DELAY_AC)
        {
            arguments >> delay;
        }
        if (arguments.fail() || sim_time&SIGN_BIT || n_of_booths&SIGN_BIT || delay&SIGN_BIT)
        {
            throw ComandLineArgumentError("Bad argument format!");
        }

        std::ofstream log_file_stream(log_file);
        if (!log_file_stream.good())
        {
            throw FileNotGoodError(log_file);
        }

        std::ifstream customers_json(customer_file);
        if (!customers_json.good())
        {
            throw FileNotGoodError(customer_file);
        }
        json customers_obj = json::parse(stream_to_string(customers_json));
        customers_json.close();

        std::ifstream disc_database_json(discs_file);
        if (!disc_database_json.good())
        {
            throw FileNotGoodError(discs_file);
        }
        json disc_database_obj = json::parse(stream_to_string(disc_database_json));
        disc_database_json.close();

        VinylDatabase database;
        EventLog log(cout, log_file_stream);
        try
        {
            database = VinylDatabase(disc_database_obj);
        }
        catch (const nlohmann::detail::type_error &e)
        {
            throw JsonMissingFieldError(discs_file, e.what());
        }

        VinylRecordShop shop(log, n_of_booths, database, sim_time);
        try
        {
            shop.loadCustomersFromJson(customers_obj);
        }
        catch (const nlohmann::detail::type_error &e)
        {
            throw JsonMissingFieldError(customer_file, e.what());
        }

        TerminalIO cmd_io(cin, cout, shop);

        cout << "[!] World loop starts\n";
        // Main simulation loop
        for (uint t = 0; t < sim_time; t++)
        {
            if (cmd_io.isBreakpoint(t))
            {
                try
                {
                    cmd_io.query(t);
                }
                catch (const TerminalIOExitSignal &e)
                {
                    cout << "[!] Exiting simulation\n";
                    break;
                }
            }
            shop.update(t);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        log_file_stream.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[!] CRITICAL ERROR: [!]\n\n"
                  << e.what() << '\n';
        return 1;
    }
    return 0;
}