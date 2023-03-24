#ifndef ACTION_H
#define ACTION_H

#include "discs/disc.h"
/**
 * @brief Class representing Action which cusomer can do in shop. Consists of Disc and its DiscAction
 *
 */
class Action
{
    private:
        std::shared_ptr<Disc> disc;
        DiscAction action;

    public:
    Action();
    /**
     * @brief Construct a new Action object
     *
     * @param disc pointer to the Disc of this Action
     * @param action DiscAction of this this Action
     */
    Action(std::shared_ptr<Disc> disc, DiscAction action);

    /**
     * @brief Returns true if Discs of two Action are the same.
     *
     */
    bool operator==(const Action& second) const;
    /**
     * @brief Returns true if Discs of two Action are diffrent.
     *
     */
    bool operator!=(const Action& second) const;

    /**
     * @brief Get pointer to the Disc
     *
     * @return std::shared_ptr<Disc>
     */
    std::shared_ptr<Disc> get_disc_ptr() const;

    /**
     * @brief Get Disc of this Action
     *
     * @return Disc&
     */
    Disc& get_disc() const;

    /**
     * @brief Get the DiscAction of this action
     *
     * @return DiscAction
     */
    DiscAction get_action() const;

    friend std::ostream &operator<<(std::ostream &os, const Action &action);
};

std::ostream &operator<<(std::ostream &os, const Action &customer);

#endif