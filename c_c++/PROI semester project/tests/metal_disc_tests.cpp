#include "../catch.hpp"
#include "../src/discs/metal_disc.h"

TEST_CASE("metal_disc tests", "[metal_disc]")
{
    Author author("Jan", "Kowalski");
    MetalDisc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 3);
        CHECK(disc.get_total_price() == 1100);
        CHECK(disc.get_base_price() == 1000);
    }
}