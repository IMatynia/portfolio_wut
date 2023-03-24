#include "../src/discs/country_disc.h"
#include "../catch.hpp"

TEST_CASE("country_disc tests", "[country_disc]")
{
    Author author("Jan", "Kowalski");
    CountryDisc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 1);
        CHECK(disc.get_total_price() == 1500);
        CHECK(disc.get_base_price() == 1000);
    }
    SECTION("operators")
    {
        Disc same_disc(author, "Music", 1000);
        Disc l_disc(author, "First Disc", 2000);
        Disc g_disc(author, "Second Disc", 500);

        CHECK(disc != same_disc);
        CHECK(disc > l_disc);
        CHECK(disc > g_disc);
    }
}