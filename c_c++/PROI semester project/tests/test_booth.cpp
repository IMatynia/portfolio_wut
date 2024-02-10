#include "../catch.hpp"
#include "../src/assistant_booth.h"
#include "../src/custom_excpetions.hpp"

TEST_CASE("Test assistant booth", "[AssistantBooth]")
{

    json data = {
        {
            {"genre_id", "reggae"},
            {"amount", 0},
            {"author", {
                    {"name", "Firstname"},
                    {"surname", "Lastname"}
                }
            },
            {"title", "Reggae"},
            {"price", 1000}
        },
        {
            {"genre_id", "country"},
            {"amount", 7},
            {"author", {
                    {"name", "Firstname"},
                    {"surname", "Lastname"}
                }
            },
            {"title", "Country"},
            {"price", 1200}
        },
        {
            {"genre_id", "rock"},
            {"amount", 1},
            {"author", {
                    {"name", "First"},
                    {"surname", "Last"}
                }
            },
            {"title", "Rock"},
            {"price", 500}
        }
    };

    std::stringstream ss;
    EventLog log(ss);
    VinylDatabase database(data);
    AssistantBooth booth(log, database);
    AssistantBooth second_booth(log, database);

    SECTION("Availability")
    {
        REQUIRE(booth.isAvailable());
        auto bob = std::make_unique<Customer>("Bob", "stevens", 100, 100, 100);

        // Puting bob inside the booth
        booth.giveCustomer(bob);
        REQUIRE_FALSE(booth.isAvailable());
        REQUIRE(bob == nullptr);

        //Taking bob out of the booth
        auto bob2 = booth.extractCustomer();
        REQUIRE(booth.isAvailable());
        REQUIRE_FALSE(bob2 == nullptr);

        //Taking something that does not exist
        CHECK_THROWS_AS(booth.extractCustomer(), BoothIsEmptyError);

        booth.giveCustomer(bob2);
        CHECK_THROWS_AS(booth.giveCustomer(bob), BoothIsNotEmptyError);
    }

    SECTION("Action process")
    {
    second_booth.update(0);
    CHECK(second_booth.isAvailable());

    for(int i=0; i<5; i++)
    {
    auto bill = std::make_unique<Customer>("Bill", "Stevens", 100, 100, 100);
    second_booth.giveCustomer(bill);

    second_booth.update(0);

    uint n_actions = second_booth.get_n_actions_to_left();
    Action first_action = second_booth.get_current_action();
    uint time = second_booth.get_time_to_finish();
    bool failedBuy = first_action.get_action() == buy && !(database.is_disc_in_stock(first_action.get_disc()));

    CHECK(second_booth.get_flag_is_processing_action());

    for (uint i=0; i<time; ++i)
    {
        second_booth.update(i);
    }
    CHECK(!(second_booth.get_flag_is_processing_action()));
    CHECK(second_booth.get_current_action() == first_action);
    second_booth.update(0);


    if (failedBuy)
    {
    CHECK(second_booth.get_n_actions_to_left() == n_actions);
    CHECK(second_booth.get_current_action().get_action() == order);
    CHECK(second_booth.get_current_action().get_disc() == first_action.get_disc());
    }
    else // n_actions > 1
    {
        CHECK(second_booth.get_current_action() != first_action);
        CHECK(second_booth.get_n_actions_to_left() == --n_actions);
    }
    second_booth.extractCustomer();
    }
    }


    json one_disc = {
                {
            {"genre_id", "reggae"},
            {"amount", 0},
            {"author", {
                    {"name", "Firstname"},
                    {"surname", "Lastname"}
                }
            },
            {"title", "Reggae"},
            {"price", 1000}
        }
        };
    std::stringstream ss2;
    EventLog log2(ss2);
    VinylDatabase single(one_disc);



    SECTION("One disc, one action")
    {

    for(int i=0; i<5; i++)
    {
    AssistantBooth third_booth(log2, single);
    auto billy = std::make_unique<Customer>("Billy", "Stevens", 100, 100, 100);
    third_booth.giveCustomer(billy);
    CHECK(third_booth.get_n_actions_to_left() == 1);
    third_booth.update(0);

    uint n_actions = third_booth.get_n_actions_to_left();
    Action action = third_booth.get_current_action();

    CHECK(n_actions == 0);

    uint time = third_booth.get_time_to_finish();
    for (uint i=0; i<=time; ++i)
    {
        third_booth.update(i);
    }

    third_booth.update(0);

    CHECK(third_booth.get_currnt_customer().getFinishedActions().size() == 1);
    CHECK(action == *(third_booth.get_currnt_customer().getFinishedActions()[0]));

    bool isFinished = ((action.get_action() == buy && !(single.is_disc_in_stock(action.get_disc())) && n_actions == 1) || n_actions == 0);
    CHECK(isFinished);

    third_booth.extractCustomer();
    };
    }

}