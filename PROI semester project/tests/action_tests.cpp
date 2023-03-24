#include "../catch.hpp"
#include "../src/discs/disc.h"
#include "../src/action.h"

TEST_CASE("Action tests", "[Action]")
{

    Author author1("Firstname", "Lastname");
    Author author2("First", "Last");
    Disc r_disc(author1, "Reggae", 1000);
    Disc c_disc(author1, "Country", 1200);
    Disc ro_disc(author2, "Rock", 500);

    Action action1(std::make_shared<Disc>(r_disc), buy);
    auto pointer2 = std::make_shared<Disc>(r_disc);
    Action action2(pointer2, order);
    Action action3(std::make_shared<Disc>(c_disc), buy);
    Action action4(std::make_shared<Disc>(ro_disc), ask);

    SECTION("check getters")
    {
        CHECK(action1.get_action() == buy);
        CHECK(action2.get_disc_ptr() == pointer2);
        CHECK(action3.get_disc() == c_disc);
    }

    SECTION("check oerators")
    {
            CHECK(action1 == action1);
            CHECK(action1 != action3);
            CHECK(action1 != action3);
            CHECK(action1 == action2);

    }

}



