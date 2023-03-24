#include "../catch.hpp"

#include <iostream>
#include "../src/event_log.h"
#include "../src/custom_events.hpp"

TEST_CASE("Test event log", "[EventLog]")
{
    std::stringstream ss;
    EventLog log(ss);

    SECTION("Example event add")
    {
        Customer steve("Steve", "Bobson", 100, 100, 5);
        CustomerEnteredShop steve_entry(100, steve);
        log.addEvent(steve_entry);
        CHECK(ss.str() == "[T=    100] => Steve Bobson (id:14) [outside a shop] entered the shop\n");
    }
}