#ifndef __ISHOP_H__
#define __ISHOP_H__

#include <memory>
#include "customer.h"

/**
 * @brief Shop interface, to define universal shop methods
 *
 */
class IShop
{
protected:
    /// Removes the first customer from the queue
    virtual void removeFirstCustomerFromQueue() = 0;
    /// Update the selected customer and add all relevant events to the event log
    virtual void updateCustomer(uint tick, Customer &customer) = 0;

    /// Heap with customers
    std::vector<std::unique_ptr<Customer>> customer_queue;
public:
    IShop() = default;
    virtual size_t getQueueSize() = 0;
    /// Moves a customer unique pointer to the queue
    virtual void addCustomerToQueue(std::unique_ptr<Customer> &new_customer) = 0;
    /// Loads all customers represented in the json list
    virtual void loadCustomersFromJson(json &customer_list_obj) = 0;
    /// Updates the entire shop at a given tick
    virtual void update(uint tick) = 0;

    /// Retrurns a reference to a customer in the queue at a given index
    virtual std::unique_ptr<Customer> &getCustomerRef(size_t index) = 0;
    /// Print people in queue to stream
    virtual std::ostream &printQueue(std::ostream &os) const = 0;

    virtual ~IShop(){};
};
#endif // __ISHOP_H__