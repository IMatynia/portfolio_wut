#include "../src/discs/author.h"
#include "../catch.hpp"
#include "../src/json.hpp"

using nlohmann::json;

TEST_CASE("Test author construction from json", "[Author, Json]")
{
    json author_obj = {{"name", "Steve"},
                       {"surname", "Johnson"}};
    Author author(author_obj);

    CHECK(author.get_first_name() == "Steve");
    CHECK(author.get_last_name() == "Johnson");
}

TEST_CASE("author tests", "[author]")
{
    Author author("Jan", "Kowalski");
    SECTION("Typical cases - getters")
    {
        CHECK(author.get_first_name() == "Jan");
        CHECK(author.get_last_name() == "Kowalski");
        CHECK(author.get_full_name() == "Jan Kowalski");
    }
    SECTION("Typical cases - setters")
    {
        author.set_first_name("John");
        author.set_last_name("Smith");

        CHECK(author.get_first_name() == "John");
        CHECK(author.get_last_name() == "Smith");
        CHECK(author.get_full_name() == "John Smith");
    }
    SECTION("Empty fields")
    {
        Author unknown("", "");

        CHECK(unknown.get_full_name() == "Unkown");

        author.set_last_name("");
        CHECK(author.get_full_name() == "Jan ");

        author.set_first_name("");
        CHECK(author.get_full_name() == "Unkown");
    }
}