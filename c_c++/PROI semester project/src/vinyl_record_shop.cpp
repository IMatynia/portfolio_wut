#include "vinyl_record_shop.h"

#include <chrono>
#include <random>

VinylRecordShop::VinylRecordShop(EventLog &new_log, uint new_n_of_assistants, VinylDatabase &new_vinyl_database, uint new_max_ticks) : log(new_log), n_of_assistants(new_n_of_assistants), vinyl_database(new_vinyl_database), max_ticks(new_max_ticks)
{
    if (new_n_of_assistants < 1)
    {
        std::invalid_argument("Number of assestants must be greater than 0");
    }
    for (size_t i = 0; i < new_n_of_assistants; i++)
    {
        auto booth = std::make_unique<AssistantBooth>(log, vinyl_database);
        all_booths.push_back(std::move(booth));
    }
}

uint VinylRecordShop::getnAssistants()
{
    return n_of_assistants;
}
uint VinylRecordShop::getMaxTicks()
{
    return max_ticks;
}
size_t VinylRecordShop::getQueueSize()
{
    return customer_queue.size();
}
void VinylRecordShop::update(uint tick)
{
    for (auto &customer : customer_queue)
    {
        updateCustomer(tick, *customer);
    }

    for (auto &booth : all_booths)
    {
        updateBooth(tick, *booth);
    }
}

std::unique_ptr<Customer> &VinylRecordShop::getCustomerRef(size_t index)
{
    if (index >= getQueueSize())
        throw std::out_of_range("Invalid index, Queue only has " + std::to_string(customer_queue.size()) + " customers");
    return customer_queue[index];
}

std::unique_ptr<AssistantBooth> &VinylRecordShop::getBoothRef(size_t index)
{
    if (index >= getnAssistants())
        throw std::out_of_range("Invalid index, Shop only has " + std::to_string(n_of_assistants) + " booths");
    return all_booths[index];
}

std::ostream &VinylRecordShop::printBooths(std::ostream &os) const
{
    for (size_t i = 0; i < all_booths.size(); i++)
    {
        os << "<" << i << "> " << *all_booths[i] << std::endl;
    }
    return os;
}

std::ostream &VinylRecordShop::printQueue(std::ostream &os) const
{
    if (customer_queue.size() > 0)
    {
        os << "First person in the queue:" << std::endl;
        customer_queue[0]->fullSummary(os);
        os << "All people in the queue:" << std::endl;
        for (size_t i = 0; i < customer_queue.size(); i++)
        {
            os << "<" << i << ">" << *customer_queue[i] << std::endl;
        }
    }
    else
    {
        os << "Queue is currently empty" << std::endl;
    }
    return os;
}

std::ostream &VinylRecordShop::printDatabase(std::ostream &os) const
{
    os << vinyl_database;
    return os;
}

std::ostream &operator<<(std::ostream &os, const VinylRecordShop &shop)
{
    os << "<Vinyl database:>" << std::endl;
    shop.printDatabase(os);

    os << "\n<Booths:>" << std::endl;
    shop.printBooths(os);

    os << "\n<Queue:>" << std::endl;
    shop.printQueue(os);

    return os;
}

void VinylRecordShop::updateCustomer(uint tick, Customer &customer)
{
    /// Bored and in queue? Mark as bored
    if (customer.getState() == state_in_shop && customer.getTExit() <= tick)
    {
        log.addEvent(CustomerGotBoredAndLeft(tick, customer));
        customer.setState(state_bored);
    }
    // it is time to enter the shop. Change state
    if (customer.getState() == state_outside && customer.getTEntry() <= tick)
    {
        log.addEvent(CustomerEnteredShop(tick, customer));
        customer.setState(state_in_shop);
    }
}

void VinylRecordShop::updateBooth(uint tick, AssistantBooth &booth)
{
    if (booth.isAvailable())
    {
        //*A new customer walks up to the booth*
        std::unique_ptr<Customer> some_customer = nullptr;

        // Reapeat as long as there are customers in the queue or you found a suitable one
        while (customer_queue.size() > 0 && customer_queue[0]->getState() >= state_in_shop && some_customer == nullptr)
        {
            some_customer = std::move(customer_queue[0]);
            removeFirstCustomerFromQueue();
            // Are you ACTUALLY in the shop or are you just a ghost?
            if (some_customer->getState() >= state_bored)
            {
                // Yes i am a ghost, get another person from the queue
                some_customer.reset();
            }
        }
        // Put them in the booth
        if (some_customer != nullptr)
        {
            log.addEvent(CustomerEnteredBooth(tick, booth.getId(), *some_customer));
            booth.giveCustomer(some_customer);
        }
        else
        {
            // The booth is empty and there are no customers that can be served
            return;
        }
    }

    // Update whatever the booth is doing
    booth.update(tick);

    // Is the customer finished with their purchase?
    if (booth.isCustomerReadyForExctraction())
    {
        std::unique_ptr<Customer> customer = booth.extractCustomer();
        log.addEvent(CustomerLeftTheShop(tick, *customer));
        customer.reset(); // Kill the customer in cold blood
    }
}

void VinylRecordShop::addCustomerToQueue(std::unique_ptr<Customer> &new_customer)
{
    CustomerPtrComparator cmp;
    customer_queue.push_back(std::move(new_customer));
    std::push_heap(customer_queue.begin(), customer_queue.end(), cmp);
}

void VinylRecordShop::loadCustomersFromJson(json &customer_list_obj)
{
    for (auto &customer_obj : customer_list_obj)
    {
        Customer randomized = Customer::generateCustomer(customer_obj["name"], customer_obj["surname"], max_ticks);
        auto customer = std::make_unique<Customer>(randomized);
        addCustomerToQueue(customer);
    }
}

void VinylRecordShop::removeFirstCustomerFromQueue()
{
    CustomerPtrComparator cmp;
    std::pop_heap(customer_queue.begin(), customer_queue.end(), cmp);
    customer_queue.pop_back();
}
