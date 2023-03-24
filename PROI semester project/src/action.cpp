#include "action.h"

Action::Action(std::shared_ptr<Disc> disc, DiscAction action): disc(disc), action(action)
{}

Action::Action()
{}


std::shared_ptr<Disc> Action::get_disc_ptr() const
{
    return disc;
}

Disc& Action::get_disc() const
{
    return *(disc);
}

DiscAction Action::get_action() const
{
    return action;
}

bool Action::operator==(const Action &second) const
{
    return (*(disc) == *(second.disc));
}

bool Action::operator!=(const Action &second) const
{
    return (!(*(this) == second));
}

std::ostream &operator<<(std::ostream &os, const Action &action)
{
    std::string action_str;
    switch (action.get_action())
    {
    case buy:
    {
        action_str = "Buying";
        break;
    }

    case order:
    {
        action_str = "Ordering";
        break;
    }

    default: //ask
    {
        action_str = "Asking about";
        break;
    }
    }
    os << action_str << " Vinyl: " << action.get_disc() << std::endl;

    return os;

}