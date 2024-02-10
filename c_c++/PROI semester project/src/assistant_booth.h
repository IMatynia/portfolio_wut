#ifndef __ASSISTANT_BOOTH_H__
#define __ASSISTANT_BOOTH_H__

#define BUY_TIME 10
#define ORDER_TIME 5
#define ASK_TIME 3
#define TIME_FACTOR 3

#include <memory>

#include "event_log.h"
#include "customer.h"
#include "vinyl_database.h"
#include "custom_events.hpp"
#include "custom_excpetions.hpp"
#include <tuple>

/**
 * @brief A class representing a booth in which a customer can place their order or get recommendations
 *
 */
class AssistantBooth
{
private:
    bool customer_ready;
    bool processing_action;
    uint time_to_finish;

    uint n_actions_to_left;

    static uint next_id;
    uint id;
    EventLog &log;
    std::unique_ptr<Customer> customer;

    VinylDatabase &vinyl_database;
    /**
     * @brief Set the  new currentAction which Booth will serve.
     *
     */
    void setNewAction();
    std::shared_ptr<Action> currentAction;
    /**
     * @brief Flag about whether this is first Action of Customer
     *
     */
    bool isFirstAction;

    /**
     * @brief Returns true if person does not have any other Actions to pass
     *
     */
    bool isCustomerDone();

    /**
     * @brief Flag about whether Customer tried to buy a Disc which is not on stock.
     *
     */
    bool failedBuyFlag;

public:
    /**
     * @brief Construct a new Assistant Booth object
     *
     * @param new_log           Log for event logging (mainly in ::update)
     * @param vinyl_database    Database access to see what discs are available/change their amounts/get prices
     */
    AssistantBooth(EventLog &new_log, VinylDatabase &vinyl_database);
    /// Updates all internal logic in a booth, namely handles actions of the held cusomer
    void update(uint tick);
    uint getId() const;
    /// Returns true/false depending on whether or not it can take a new customer
    bool isAvailable() const;
    /// Returns true if the customer in the booth is done with their orders or got bored
    bool isCustomerReadyForExctraction() const;
    /// Method for passing a new customer into the booth (a new person walks up to the booth)
    void giveCustomer(std::unique_ptr<Customer> &new_customer);
    /// Removes the current customer if there is one and returns it
    std::unique_ptr<Customer> extractCustomer();

    /// Finish the currentAction, by adding new Event to Event Log and set the proper flags
    void finishAction(uint id);

    Action get_current_action() const;
    VinylDatabase get_vinyl_database() const;
    Customer get_currnt_customer() const;
    /// Get time until the action will be finished.
    uint get_time_to_finish() const;

    /// Get flag whether the Booth is donig some Action
    bool get_flag_is_processing_action() const;
    /// Get flag if customer does not have any other action to pass
    bool get_flag_is_customer_ready() const;
    bool get_flag_is_first_action() const;
    uint get_n_actions_to_left() const;


    /// one line summary printed onto an ostream
    std::ostream &shortSummary(std::ostream &os) const;
    /// full summary printed onto an ostream
    std::ostream &fullSummary(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const AssistantBooth &booth);
    ~AssistantBooth(){};
};

/// the same as AssistantBooth::shortSummary
std::ostream &operator<<(std::ostream &os, const AssistantBooth &booth);

#endif // __ASSISTANT_BOOTH_H__