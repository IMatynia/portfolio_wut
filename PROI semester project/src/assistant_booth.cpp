#include "assistant_booth.h"
#include "vinyl_record_shop.h"
#include "utilities.h"
#include "action.h"
#include "discs/disc_not_found_error.h"

uint AssistantBooth::next_id = 0;

AssistantBooth::AssistantBooth(EventLog &new_log, VinylDatabase &vinyl_database) : log(new_log), id(next_id++), vinyl_database(vinyl_database)
{
    customer = nullptr;
    currentAction = nullptr;
    customer_ready = false;
    processing_action = false;
    isFirstAction = true;
    time_to_finish = 0;
    failedBuyFlag = false;

    if (BUY_TIME <= 0)
    {
        throw(NonPositiveValueError("BUY_TIME"));
    }
    if (ORDER_TIME <= 0)
    {
        throw(NonPositiveValueError("ORDER_TIME"));
    }
    if (ASK_TIME <= 0)
    {
        throw(NonPositiveValueError("ASK_TIME"));
    }
    if (TIME_FACTOR <= 0)
    {
        throw(NonPositiveValueError("TIME_FACTOR"));
    }
}

void AssistantBooth::update(uint tick)
{
    if (customer == nullptr)
    {
        return;
    }

    if (customer->getState() >= state_bored)
    {
        customer_ready == true;
        return;
    }
    if (!(processing_action))
    {
        setNewAction();
    }
    --time_to_finish;
    if (time_to_finish == 0)
    {
        finishAction(tick);
    }
}

uint AssistantBooth::getId() const
{
    return id;
}

bool AssistantBooth::isAvailable() const
{
    return customer.get() == nullptr;
}

bool AssistantBooth::isCustomerReadyForExctraction() const
{
    return customer_ready;
}

void AssistantBooth::giveCustomer(std::unique_ptr<Customer> &new_customer)
{
    if (customer != nullptr)
    {
        throw BoothIsNotEmptyError(id);
    }
    customer = std::move(new_customer);
    n_actions_to_left = customer->getnActions() > vinyl_database.get_shop_stock().size() ? vinyl_database.get_shop_stock().size() : customer->getnActions();
    isFirstAction = true;
}

std::unique_ptr<Customer> AssistantBooth::extractCustomer()
{
    if (customer == nullptr)
    {
        throw BoothIsEmptyError(id);
    }
    customer_ready = false;
    return std::move(customer);
}

void AssistantBooth::finishAction(uint tick)
{
    switch (currentAction->get_action())
    {
    case ask:
        if (vinyl_database.is_disc_in_stock(currentAction->get_disc()))
        {
            log.addEvent(CustomerAskedAboutVinyl(tick, id, *customer, currentAction->get_disc(), avalible_to_buy));
        }
        else
        {
            log.addEvent(CustomerAskedAboutVinyl(tick, id, *customer, currentAction->get_disc(), avalible_to_order));
        }
        break;
    case order:
    {
        if (vinyl_database.is_disc_in_database(currentAction->get_disc()))
        {
            log.addEvent(CustomerOrderedAVinyl(tick, id, *customer, currentAction->get_disc()));
            customer->addMoneySpent(currentAction->get_disc().get_total_price());
        }
        else
        {
            throw DiscNotFoundError(currentAction->get_disc().get_title());
        }
        break;
    }
    case buy:
    {

        switch (vinyl_database.sell_disc(currentAction->get_disc()))
        {
        case avalible_to_buy:
        {
            // add customer disc to bill
            log.addEvent(CustomerBoughtVinyl(tick, id, *customer, currentAction->get_disc(), avalible_to_buy));
            customer->addMoneySpent(currentAction->get_disc().get_total_price());
            // we do not have to remove disc from database here; it is removed in .sell_disc()
            break;
        }

        case avalible_to_order:
        {
            log.addEvent(CustomerBoughtVinyl(tick, id, *customer, currentAction->get_disc(), avalible_to_order));
            failedBuyFlag = true;
            ++n_actions_to_left;
            break;
        }

        default: // not_in_database
        {
            throw DiscNotFoundError(currentAction->get_disc().get_title());
            break;
        }
        }
    }
    break;

    default:
        break;
    }
    customer->addFinishedAction(currentAction);
    processing_action = false;
}

std::ostream &AssistantBooth::shortSummary(std::ostream &os) const
{
    os << "ID:" << id << " | Current task:";
    if (currentAction)
    {
        os << *currentAction;
    }
    else
    {
        os << "No action";
    }
    os << " | Customer served: ";
    if (customer)
    {
        customer->shortSummary(os);
    }
    else
    {
        os << "Booth available";
    }
    return os;
}

std::ostream &AssistantBooth::fullSummary(std::ostream &os) const
{
    os << "ID: " << id << std::endl;
    if (customer == nullptr)
    {
        os << "Booth available" << std::endl;
    }
    else
    {
        os << "Current task: ";
        if (currentAction)
        {
            os << *currentAction;
        }
        else
        {
            os << "No action";
        }
        os << std::endl;

        os << "Customer served: ";
        customer->fullSummary(os) << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const AssistantBooth &booth)
{
    return booth.shortSummary(os);
}

void AssistantBooth::setNewAction()
{
    if (!(isCustomerDone()))
    {
        int firstActionOffset = (isFirstAction) ? 1 : 0;
        if (isFirstAction)
        {
            isFirstAction = false;
        }
        if (!(failedBuyFlag))
        {
            Action new_action;
            do
            {
                new_action = Action(vinyl_database.get_random_disc_from_database(), get_random_disc_action());
            } while (customer->wasDoneInThePast(new_action));

            currentAction = std::make_shared<Action>(new_action);
        }
        else
        {
            Action new_action(currentAction->get_disc_ptr(), order);
            currentAction = std::make_shared<Action>(new_action);
            failedBuyFlag = false;
        }

        // customer->setnActions(customer->getnActions() - 1);
        --n_actions_to_left;

        processing_action = true;
        switch (currentAction->get_action())
        {
        case buy:
            time_to_finish = BUY_TIME + randint(TIME_FACTOR) + firstActionOffset;
            break;

        case order:
            time_to_finish = ORDER_TIME + randint(TIME_FACTOR) + firstActionOffset;
            break;

        default: // ask
            time_to_finish = ASK_TIME + randint(TIME_FACTOR) + firstActionOffset;
            break;
        }
    }
    else
    {
        // customer passed all discs
        customer_ready = true;
    }
}

bool AssistantBooth::isCustomerDone()
{
    return (n_actions_to_left == 0);
}

Action AssistantBooth::get_current_action() const
{
    return *(currentAction);
}

VinylDatabase AssistantBooth::get_vinyl_database() const
{
    return vinyl_database;
}

Customer AssistantBooth::get_currnt_customer() const
{
    return *(customer);
}

uint AssistantBooth::get_time_to_finish() const
{
    return time_to_finish;
}

bool AssistantBooth::get_flag_is_processing_action() const
{
    return processing_action;
}

bool AssistantBooth::get_flag_is_customer_ready() const
{
    return customer_ready;
}

bool AssistantBooth::get_flag_is_first_action() const
{
    return isFirstAction;
}

uint AssistantBooth::get_n_actions_to_left() const
{
    return n_actions_to_left;
}
