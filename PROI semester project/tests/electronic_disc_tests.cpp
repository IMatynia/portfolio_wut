#include "../catch.hpp"
#include "../src/discs/electronic_disc.h"

TEST_CASE("electronic_disc tests", "[electronic_disc]")
{
    Author author("Jan", "Kowalski");
    ElectronicDisc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 5);
        CHECK(disc.get_total_price() == 1050);
        CHECK(disc.get_base_price() == 1000);
    }
}