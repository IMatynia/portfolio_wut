#include "../catch.hpp"
#include "../src/vinyl_record_shop.h"

TEST_CASE("Shop creation")
{
    std::stringstream ss;
    EventLog log(ss);
    json discs_json = {
        {{"genre_id", "country"},
         {"author", {{"name", "President"}, {"surname", "OfAzerbeijan"}}},
         {"title", "Country of Azerbeijan"},
         {"price", 100.0}}};
    json customers = {
        {{"name", "Alfred"},
         {"surname", "Butimen"}},
        {{"name", "Cecylia"},
         {"surname", "Delegovska"}}};
    VinylDatabase database(discs_json);
    VinylRecordShop shop(log, 3, database, 100);
    shop.loadCustomersFromJson(customers);

    SECTION("Getters")
    {
        CHECK(shop.getBoothRef(0)->isAvailable());
        CHECK(shop.getBoothRef(1)->isAvailable());
        CHECK(shop.getMaxTicks() == 100);
        CHECK(shop.getnAssistants() == 3);
        CHECK(shop.getQueueSize() == 2);
        CHECK_THROWS_AS(shop.getBoothRef(100), std::out_of_range);
        CHECK_THROWS_AS(shop.getCustomerRef(100), std::out_of_range);
    }

    SECTION("Add customers")
    {
        auto guy = std::make_unique<Customer>("George", "Washingmachine", 50, 100, 5);
        shop.addCustomerToQueue(guy);
        CHECK(shop.getQueueSize() == 3);
    }
}