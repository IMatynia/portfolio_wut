#include "customer.h"
#include "discs/disc.h"
#include "vinyl_record_shop.h"

uint Customer::next_id = 0;

Customer::Customer(string new_name, string new_surname, uint new_time_of_entry, uint new_time_to_leave, uint new_action_count) : Person(new_name, new_surname), t_to_leave(new_time_to_leave), id(next_id++), current_state(state_outside), t_of_entry(new_time_of_entry), t_of_exit(t_of_entry + t_to_leave), money_spent(0), n_of_actions(new_action_count)
{
}

Customer Customer::generateCustomer(string new_name, string new_surname, uint max_ticks)
{
    max_ticks = std::max(max_ticks, uint(MIN_TIME_IN_SHOP));
    uint n_of_actions = randint_range(1, MAX_ACTION_COUNT);
    uint new_time_of_entry = randint_range(0, max_ticks - MIN_TIME_IN_SHOP);
    uint ttl = randint_range(MIN_TIME_IN_SHOP, max_ticks - new_time_of_entry + MIN_TIME_IN_SHOP);
    return Customer(new_name, new_surname, new_time_of_entry, ttl, n_of_actions);
}

uint Customer::getTTL() const
{
    return t_to_leave;
}

uint Customer::getTEntry() const
{
    return t_of_entry;
}

uint Customer::getTExit() const
{
    return t_of_exit;
}

uint Customer::getnActions() const
{
    return n_of_actions;
}

void Customer::setnActions(uint new_action_amount)
{
    n_of_actions = new_action_amount;
}

uint Customer::getMoneySpent() const
{
    return money_spent;
}

void Customer::addMoneySpent(uint amount)
{
    money_spent += amount;
}

CustomerStates Customer::getState() const
{
    return current_state;
}

void Customer::setState(CustomerStates new_state)
{
    if (new_state <= current_state)
    {
        throw WrongStateOrderError(id, state_to_string[new_state], state_to_string[current_state]);
    }
    current_state = new_state;
}

const string Customer::state_to_string[4] = {
    "outside a shop",
    "inside a shop",
    "got bored and left",
    "left the shop"};

std::string Customer::get_total_money_spent_str() const
{
    uint main_currency = getMoneySpent() / 100;
    uint minor_currency = getMoneySpent() % 100;
    std::string add_zero = minor_currency == 0 ? "0" : "";
    return std::to_string(main_currency) + "," + std::to_string(minor_currency) + add_zero;
}
void Customer::addFinishedAction(std::shared_ptr<Action> action)
{
    finishedActions.push_back(action);
}

std::vector<std::shared_ptr<Action>> Customer::getFinishedActions() const
{
    return finishedActions;
}

bool Customer::wasDoneInThePast(Action action)
{
    for (auto done_action : finishedActions)
    {
        if (*(done_action) == action)
        {
            return true;
        }
    }
    return false;
}

std::ostream &Customer::shortSummary(std::ostream &os) const
{
    os << get_full_name() << " (id:" << id << ")";
    if (current_state != state_in_shop)
    {
        os << " [" << state_to_string[current_state] << "]";
    }
    return os;
}

std::ostream &Customer::fullSummary(std::ostream &os) const
{
    shortSummary(os) << std::endl;
    os << " - Money spent: " << get_total_money_spent_str() << std::endl;
    os << " - State: " << state_to_string[current_state] << std::endl;
    os << " - Enters/Bores at: " << t_of_entry << "/" << t_of_exit << std::endl;
    os << " - Number of actions the customer wants to do: " << n_of_actions << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Customer &customer)
{
    return customer.shortSummary(os);
}
