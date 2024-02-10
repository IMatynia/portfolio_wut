#include "../catch.hpp"
#include "../src/discs/hip_hop_disc.h"

TEST_CASE("hip_hop_disc tests", "[hip_hop_disc]")
{
    Author author("Jan", "Kowalski");
    HipHopDisc disc(author, "Music", 1000);
    SECTION("chceck getters")
    {
        CHECK(disc.get_type_id() == 7);
        CHECK(disc.get_total_price() == 1100);
        CHECK(disc.get_base_price() == 1000);
    }
}