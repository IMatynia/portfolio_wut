#ifndef __CUSTOMER_H__
#define __CUSTOMER_H__

#define MAX_ACTION_COUNT 4
#define MIN_TIME_IN_SHOP 20

#include <iostream>
#include <algorithm>
#include "person.h"
#include "json.hpp"
#include "custom_excpetions.hpp"
#include "discs/disc.h"
#include "utilities.h"
#include "action.h"
#include <tuple>

/// Enum for states that a customer can be in
enum CustomerStates
{
    state_outside,
    state_in_shop,
    state_bored,
    state_left_shop
};

using nlohmann::json;
using std::string;

/// Represents a customer who will buy records in the shop
class Customer: public Person
{
private:
    static uint next_id;
    const uint id;

    uint t_to_leave;
    uint t_of_entry;
    uint t_of_exit;

    CustomerStates current_state;

    uint money_spent;
    uint n_of_actions;
    std::vector<std::shared_ptr<Action>> finishedActions;

public:
    /**
     * @brief Construct a new Customer object
     *
     * @param new_name              Name of the customer
     * @param new_surname           Surname of the customer
     * @param new_time_of_entry     The timestamp of entry into the shop
     * @param new_time_to_leave     Maximum time a customer can spend in the shop, after that he will get bored
     *
     */
    Customer(string new_name, string new_surname, uint new_time_of_entry, uint new_time_to_leave, uint new_action_count);
    /**
     * @brief Creates a randomized customer
     *
     * @param new_name      Name of the customer
     * @param new_surname   Surname of the customer
     * @param max_ticks     The simulation time
     * @return Customer     The randomized customer object
     */
    static Customer generateCustomer(string new_name, string new_surname, uint max_ticks);

    void decremente_action_amount();
    uint getTTL() const;
    /// Returns the timestamp of customers entry to the shop
    uint getTEntry() const;
    /// Returns the timestamp at which the customer gets bored
    uint getTExit() const;

    /// Returns the amount of random actions the customer still wants to do in the shop
    uint getnActions() const;
    /// Sets the amount of random actions the customer still wants to do in the shop
    void setnActions(uint new_action_amount);

    /// Returns the raw amount of money spent
    uint getMoneySpent() const;
    /// Returns money spent in a formatted string
    std::string get_total_money_spent_str() const;
    /// Adds money to the bill of the customer
    void addMoneySpent(uint amount);

    void addFinishedAction(std::shared_ptr<Action> finished_action);
    std::vector<std::shared_ptr<Action>> getFinishedActions() const;
    bool wasDoneInThePast(Action action);

    /**
     * @brief Get the State of the object
     * ### The states can be as follows:
     *  - 0 -> not in the shop yet
     *  - 1 -> inside the shop
     *  - 2 -> customer got bored and left
     *  - 3 -> customer left the shop nominally
     * @return CustomerStates
     */
    CustomerStates getState() const;
    /// Sets the current state of the customer
    void setState(CustomerStates new_state);
    /// Lookup table for state strings
    const static string state_to_string[4];

    /// one line summary of the object
    std::ostream &shortSummary(std::ostream &os) const;
    /// full info about the object
    std::ostream &fullSummary(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const Customer &customer);
};

/// The same as Customer::shortSummary
std::ostream &operator<<(std::ostream &os, const Customer &customer);

/**
 * @brief Used as a comparator for STL containers with Customer Unique PTRs. Sorts acording to the time of entry.
 *
 */
class CustomerPtrComparator
{
public:
    /**
     * @brief Comparison operator
     *
     * @param customer_a
     * @param customer_b
     * @return true     customer A enters the shop after customer B
     * @return false
     */
    bool operator()(std::unique_ptr<Customer> &customer_a, std::unique_ptr<Customer> &customer_b)
    {
        return customer_a->getTEntry() > customer_b->getTEntry();
    }
};

#endif // __CUSTOMER_H__