#ifndef __VINYL_RECORD_SHOP_H__
#define __VINYL_RECORD_SHOP_H__

#include <iostream>
#include <vector>
#include <memory>

#include "customer.h"
#include "assistant_booth.h"
#include "event_log.h"
#include "vinyl_database.h"
#include "ishop.hpp"

/**
 * @brief A shop object, handles all relevant functionalities and simulates its contents with VinylRecordShop::update method
 *
 */
class VinylRecordShop : public IShop
{
private:
    /// Holds booth with assistants that take care of customers
    std::vector<std::unique_ptr<AssistantBooth>> all_booths;
    VinylDatabase &vinyl_database;
    EventLog &log;
    const uint n_of_assistants;
    const uint max_ticks;

    /// Removes the first customer from the queue
    void removeFirstCustomerFromQueue() override;
    /// Update the selected customer and add all relevant events to the event log
    void updateCustomer(uint tick, Customer &customer) override;
    /// Update the selected booth and add all relevant events to the log
    void updateBooth(uint tick, AssistantBooth &booth);

public:
    /**
     * @brief Construct a new Vinyl Record Shop object
     *
     * @param new_log               log to which events will be given
     * @param new_n_of_assistants   amount of booths that will take care of the customers
     * @param new_vinyl_database    database with all vinyl discs
     */
    VinylRecordShop(EventLog &new_log, uint new_n_of_assistants, VinylDatabase &new_vinyl_database, uint new_max_ticks);

    uint getnAssistants();
    uint getMaxTicks();
    size_t getQueueSize() override;

    /// Moves a customer unique pointer to the queue
    void addCustomerToQueue(std::unique_ptr<Customer> &new_customer) override;
    /// Loads all customers represented in the json list
    void loadCustomersFromJson(json &customer_list_obj) override;
    /// Updates the entire shop at a given tick
    void update(uint tick) override;

    /// Retrurns a reference to a customer in the queue at a given index
    std::unique_ptr<Customer> &getCustomerRef(size_t index) override;
    /// Returns a reference to a booth at given index
    std::unique_ptr<AssistantBooth> &getBoothRef(size_t index);

    /// Prints all booths and their short summary to stream
    std::ostream &printBooths(std::ostream &os) const;
    /// Print people in queue to stream
    std::ostream &printQueue(std::ostream &os) const override;
    /// Print database to sream
    std::ostream &printDatabase(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const VinylRecordShop &shop);

    ~VinylRecordShop() {};
};

/// Prints all relevant information to the stream
std::ostream &operator<<(std::ostream &os, const VinylRecordShop &shop);

#endif // __VINYL_RECORD_SHOP_H__