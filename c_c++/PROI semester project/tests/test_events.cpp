#include "../catch.hpp"
#include "../src/custom_events.hpp"

TEST_CASE("Testing base event class")
{
    Event ev(1234);
    std::stringstream ss("");
    SECTION("Getters")
    {
        CHECK(ev.getTimestamp() == 1234);
    }

    SECTION("<< operator")
    {
        ss << ev;
        CHECK(ss.str() == "[T=   1234] => ");
    }
}