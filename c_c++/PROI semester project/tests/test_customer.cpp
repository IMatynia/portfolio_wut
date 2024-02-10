#include "../catch.hpp"
#include "../src/customer.h"
#include "../src/custom_excpetions.hpp"

TEST_CASE("Testing customer creation", "[Customer]")
{
    Customer garry("Garry", "Holystone", 10, 40, 4);

    SECTION("Getters")
    {
        CHECK(garry.getMoneySpent() == 0);
        CHECK(garry.getnActions() == 4);
        CHECK(garry.get_first_name() == "Garry");
        CHECK(garry.get_last_name() == "Holystone");
        CHECK(garry.getState() == state_outside);
        CHECK(garry.getTEntry() == 10);
        CHECK(garry.getTTL() == 40);
        CHECK(garry.getTExit() == 50);
    }

    SECTION("Setters")
    {

        garry.setState(state_bored);
        CHECK(garry.getState() == state_bored);

        CHECK_THROWS_AS(garry.setState(state_in_shop), WrongStateOrderError);
    }

    SECTION("Money")
    {
        garry.addMoneySpent(50);
        CHECK(garry.getMoneySpent() == 50);

        CHECK(garry.get_total_money_spent_str() == "0,50");

        garry.addMoneySpent(120);
        CHECK(garry.get_total_money_spent_str() == "1,70");
    }
}