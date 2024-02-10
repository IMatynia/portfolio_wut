#include "../catch.hpp"
#include "../src/discs/classical_disc.h"

TEST_CASE("classical_disc tests", "[classical_disc]")
{
    Author author("Jan", "Kowalski");
    ClassicalDisc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 6);
        CHECK(disc.get_total_price() == 1400);
        CHECK(disc.get_base_price() == 1000);
    }
}