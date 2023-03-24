#include "../catch.hpp"
#include "../src/discs/disc.h"
#include "../src/discs/negative_price_error.h"
#include"../src/discs/empty_title_error.h"

TEST_CASE("disc tests", "[disc]")
{
    Author author("Jan", "Kowalski");
    Author alt_author("John", "Smith");
    Disc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 0);
        CHECK(disc.get_total_price() == 1000);
        CHECK(disc.get_base_price() == 1000);
        CHECK(disc.get_author().get_full_name() == "Jan Kowalski");
        CHECK(disc.get_title() == "Music");
    }
    SECTION("chceck setters")
    {
        disc.set_author(alt_author);
        disc.set_base_price(1500);

        CHECK(disc.get_author().get_full_name() == "John Smith");
        CHECK(disc.get_base_price() == 1500);
    }
    SECTION("operators")
    {
        Disc same_disc(author, "Music", 1000);
        Disc l_disc(author, "First Disc", 2000);
        Disc g_disc(author, "Second Disc", 500);

        CHECK(disc == same_disc);
        CHECK(disc != l_disc);
        CHECK(disc > l_disc);
        CHECK(disc < g_disc);
        CHECK(disc >= l_disc);
        CHECK(disc <= g_disc);
    }

    SECTION("Wrong inputs")
    {
        CHECK_THROWS_AS(Disc(author, "Ordinary", -1000), NegavitePriceError);
        CHECK_THROWS_AS(Disc(author, "", 1000), EmptyTitleError);

        CHECK_THROWS_AS(disc.set_base_price(-2000), NegavitePriceError);
    }

}