#ifndef __CUSTOM_EVENTS_H__
#define __CUSTOM_EVENTS_H__

#include "event.h"
#include "customer.h"
#include "vinyl_database.h"

class CustomerEnteredShop : public Event
{
private:
    Customer customer;

public:
    CustomerEnteredShop(int new_timestamp, Customer new_customer) : Event(new_timestamp), customer(new_customer) {}
    virtual std::ostream &printToStream(std::ostream &os) const
    {
        Event::printToStream(os) << customer << " entered the shop";
        return os;
    }
};

class CustomerLeftTheShop : public Event
{
private:
    Customer customer;

public:
    CustomerLeftTheShop(int new_timestamp, Customer new_customer) : Event(new_timestamp), customer(new_customer) {}
    virtual std::ostream &printToStream(std::ostream &os) const
    {
        Event::printToStream(os) << customer << " left the shop, they spent " << "$ " << customer.get_total_money_spent_str();
        return os;
    }
};

class CustomerGotBoredAndLeft : public Event
{
private:
    Customer &customer;

public:
    CustomerGotBoredAndLeft(int new_timestamp, Customer &new_customer) : Event(new_timestamp), customer(new_customer) {}
    virtual std::ostream &printToStream(std::ostream &os) const
    {
        Event::printToStream(os)  << customer << " left the shop prematurely because of too long waiting time in the queue";
        return os;
    }
};

class CustomerEnteredBooth : public Event
{
private:
    uint booth_id;
    Customer &customer;

public:
    CustomerEnteredBooth(uint new_timestamp, uint new_booth_id, Customer &new_customer) : Event(new_timestamp), booth_id(new_booth_id), customer(new_customer) {}
    virtual std::ostream &printToStream(std::ostream &os) const
    {
        Event::printToStream(os) << customer << " entered a booth with id =" << booth_id;
        return os;
    }
};

class CustomerAskedAboutVinyl : public Event
{
private:
    uint booth_id;
    Customer &customer;
    Disc& disc;
    DiscStatus response;

public:
    CustomerAskedAboutVinyl(uint new_timestamp, uint new_booth_id, Customer &new_customer, Disc& disc, DiscStatus response): Event(new_timestamp), booth_id(new_booth_id), customer(new_customer), disc(disc), response(response) {}
    virtual std::ostream &printToStream(std::ostream &os) const
    {
        std::string additional_response;
        if (response == avalible_to_buy)
            {
                additional_response = "This vinyl is avalible to buy in shop";
            }
        else //avalible_to_order
            {
                additional_response = "This vinyl is avalible to order";
            }

        Event::printToStream(os) << customer << " asked about a vinyl: " << disc << " in booth with id = " << booth_id << ". " << additional_response;
        return os;
    }
};


class CustomerOrderedAVinyl : public Event
{
private:
    uint booth_id;
    Customer &customer;
    Disc& disc;

public:
    CustomerOrderedAVinyl(uint new_timestamp, uint new_booth_id, Customer &new_customer, Disc& disc): Event(new_timestamp), booth_id(new_booth_id), customer(new_customer), disc(disc) {}
    virtual std::ostream &printToStream(std::ostream &os) const
    {
        Event::printToStream(os) << customer << ", which is being served in booth with id = " << booth_id << ", ordered a vinyl " << disc;
        return os;
    }
};

class CustomerBoughtVinyl : public Event
{
private:
    uint booth_id;
    Customer &customer;
    Disc& disc;
    DiscStatus response;

public:
    CustomerBoughtVinyl(int new_timestamp, uint new_booth_id, Customer &new_customer, Disc& disc, DiscStatus response): Event(new_timestamp), booth_id(new_booth_id), customer(new_customer), disc(disc), response(response) {}
    virtual std::ostream &printToStream(std::ostream &os) const
    {
        std::string additional_response;
        if (response == avalible_to_buy)
        {
            Event::printToStream(os) << customer << ", which is being served in booth with id = " << booth_id << ", bought a vinyl: " << disc;
        }
        else //avalible_to_order
        {
            Event::printToStream(os) << customer << ", which is being served in booth with id = " << booth_id << ", wanted to buy a vinyl: " << disc << ", but it was out of stock, so they will order it";

        }
        return os;
    }
};

#endif // __CUSTOM_EVENTS_H__