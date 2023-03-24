#include "../catch.hpp"
#include "../src/discs/jazz_disc.h"

TEST_CASE("jazz_disc tests", "[jazz_disc]")
{
    Author author("Jan", "Kowalski");
    JazzDisc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 4);
        CHECK(disc.get_total_price() == 1450);
        CHECK(disc.get_base_price() == 1000);
    }
}