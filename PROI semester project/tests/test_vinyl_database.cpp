#include"../catch.hpp"
#include"../src/vinyl_database.h"
#include"../src/discs/disc.h"
#include"../src/discs/country_disc.h"
#include"../src/discs/rock_disc.h"
#include"../src/discs/reggae_disc.h"
#include"../src/discs/author.h"
#include"../src/discs/duplicate_disc_error.h"
#include"../src/discs/disc_not_found_error.h"
#include"../src/custom_excpetions.hpp"

#include<set>

TEST_CASE("vinyl database tests", "[vinyl_database]")
{

    json obj = {
        {
            {"genre_id", "reggae"},
            {"author", {
                    {"name", "Firstname"},
                    {"surname", "Lastname"}
                }
            },
            {"title", "Reggae"},
            {"price", 1000}
        },
        {
            {"genre_id", "country"},
            {"author", {
                    {"name", "Firstname"},
                    {"surname", "Lastname"}
                }
            },
            {"title", "Country"},
            {"price", 1200}
        },
        {
            {"genre_id", "rock"},
            {"author", {
                    {"name", "First"},
                    {"surname", "Last"}
                }
            },
            {"title", "Rock"},
            {"price", 500}
        }
    };
    VinylDatabase disc_database(obj);

    Author author1("Firstname", "Lastname");
    Author author2("First", "Last");
    ReggaeDisc r_disc(author1, "Reggae", 1000);
    CountryDisc c_disc(author1, "Country", 1200);
    RockDisc ro_disc(author2, "Rock", 500);
    RockDisc not_in_db(author2, "404 - not found", 200);

    SECTION("check construdtor adding")
    {

        // CHECK(disc_database.is_disc_in_database(r_disc));
        CHECK(disc_database.is_disc_in_database(c_disc));
        CHECK(disc_database.is_disc_in_database(ro_disc));
        CHECK(!(disc_database.is_disc_in_database(not_in_db)));

        CHECK(disc_database.is_disc_in_database(rock_music, "Rock"));
        CHECK(!(disc_database.is_disc_in_database(rock_music, "404 - not found")));

        CHECK(disc_database.get_amount_of_disc(r_disc) <= MAX_DISC_AMOUNT);
        CHECK(disc_database.get_amount_of_disc(r_disc) >= 0);

        if (disc_database.get_amount_of_disc(r_disc) != 0)
        {
            CHECK((disc_database.is_disc_in_stock(r_disc)));
        }
        else
        {
            CHECK(!(disc_database.is_disc_in_stock(r_disc)));
        }

        CHECK(disc_database.get_disc(reggae_music, "Reggae") == r_disc);
        CHECK(disc_database.get_disc(rock_music, "Rock") == ro_disc);
        CHECK(disc_database.get_disc(country_music, "Country") == c_disc);

        int previousAmount = disc_database.get_amount_of_disc(c_disc);
        disc_database.sell_disc(c_disc);

        if (previousAmount != 0)
        {
            CHECK(disc_database.get_amount_of_disc(c_disc) == previousAmount - 1);
        }
        else
        {
            CHECK(disc_database.get_amount_of_disc(c_disc) == 0);
        }

        previousAmount = disc_database.get_amount_of_disc(ro_disc);
        disc_database.sell_disc(rock_music, "Rock");
        if (previousAmount != 0)
        {
            CHECK(disc_database.get_amount_of_disc(ro_disc) == previousAmount - 1);
        }
        else
        {
            CHECK(disc_database.get_amount_of_disc(ro_disc) == 0);
        }

        if (disc_database.get_amount_of_disc(ro_disc) != 0)
        {
             CHECK(disc_database.sell_disc(ro_disc) == avalible_to_buy);
        }
        else
        {
            CHECK(disc_database.sell_disc(ro_disc) == avalible_to_order);
        }


        Disc& random_disc = *(disc_database.get_random_disc_from_database());

        bool is_proper = (random_disc == ro_disc || random_disc == r_disc || random_disc == c_disc);
        assert(is_proper);


        CHECK_THROWS_AS(disc_database.sell_disc(not_in_db), DiscNotFoundError);


    }

    SECTION("errors")
    {
        json duplicate = {
        {
            {"genre_id", "country"},
            {"author", {
                    {"name", "Steven"},
                    {"surname", "Bobkowski"}
                }
            },
            {"title", "It's hip to be square"},
            {"price", 100}
        },
        {
            {"genre_id", "rock"},
            {"author", {
                    {"name", "Karl"},
                    {"surname", "Yungchin"}
                }
            },
            {"title", "Symphony no.69"},
            {"price", 250}
        },
        {
            {"genre_id", "reggae"},
            {"author", {
                    {"name", "Steven"},
                    {"surname", "Bobkowski"}
                }
            },
            {"title", "It's hip to be square"},
            {"price", 100}
        },
        {
            {"genre_id", "rock"},
            {"author", {
                    {"name", "Karl"},
                    {"surname", "Yungchin"}
                }
            },
            {"title", "Symphony no.69"},
            {"price", 250}
        }

        };

        json null = {};

        CHECK_THROWS_AS(VinylDatabase(duplicate), DuplicateDiscError);
        CHECK_THROWS_AS(VinylDatabase(null), EmptyDatabaseError);
        CHECK_THROWS_AS(disc_database.is_disc_in_stock(not_in_db), DiscNotFoundError);
        CHECK_THROWS_AS(disc_database.get_amount_of_disc(not_in_db), DiscNotFoundError);

    }

}