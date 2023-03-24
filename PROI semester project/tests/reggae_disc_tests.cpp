#include "../catch.hpp"
#include "../src/discs/reggae_disc.h"

TEST_CASE("reggae_disc tests", "[reggae_disc]")
{
    Author author("Jan", "Kowalski");
    ReggaeDisc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 8);
        CHECK(disc.get_total_price() == 1050);
        CHECK(disc.get_base_price() == 1000);
    }
}