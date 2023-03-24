#include "discs/disc.h"
#include "discs/country_disc.h"
#include "discs/rock_disc.h"
#include "discs/metal_disc.h"
#include "discs/jazz_disc.h"
#include "discs/electronic_disc.h"
#include "discs/classical_disc.h"
#include "discs/hip_hop_disc.h"
#include "discs/reggae_disc.h"
#include "discs/duplicate_disc_error.h"
#include "discs/disc_not_found_error.h"

#include "vinyl_record_shop.h"
#include "vinyl_database.h"
#include "custom_excpetions.hpp"
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>


VinylDatabase::VinylDatabase(json &all_discs)
{
    if (MAX_DISC_AMOUNT <= 0)
    {
        throw(NonPositiveValueError("MAX_DISC_AMOUNT"));
    }
    for (auto &disc_description : all_discs)
    {
        MusicCategories genre = Disc::str_to_genre[disc_description["genre_id"]];
        Author author(disc_description["author"]);
        std::string title = disc_description["title"];
        int price = disc_description["price"];

        uint amount = randint(MAX_DISC_AMOUNT);

        switch (genre)
        {
        case country_music:
            add_country_disc(author, title, price, amount);
            break;

        case rock_music:
            add_rock_disc(author, title, price, amount);
            break;

        case metal_music:
            add_metal_disc(author, title, price, amount);
            break;

        case jazz_music:
            add_jazz_disc(author, title, price, amount);
            break;

        case electronic_music:
            add_electronic_disc(author, title, price, amount);
            break;

        case classical_music:
            add_classical_disc(author, title, price, amount);
            break;

        case hip_hop_music:
            add_hip_hop_disc(author, title, price, amount);
            break;

        case reggae_music:
            add_reggae_disc(author, title, price, amount);
            break;

        default:
            throw InvalidMusicGenreError(genre);
            break;
        }
    }

    if (shop_stock.size() == 0)
    {
        throw EmptyDatabaseError();
    }
}

std::map<std::shared_ptr<Disc>, uint> &VinylDatabase::get_shop_stock()
{
    return shop_stock;
}

void VinylDatabase::add_country_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<CountryDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(country_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Country", new_title);
    }
}

void VinylDatabase::add_rock_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<RockDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(rock_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Rock", new_title);
    }
}

void VinylDatabase::add_metal_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<MetalDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(metal_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Metal", new_title);
    }
}
void VinylDatabase::add_jazz_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<JazzDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(jazz_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Jazz", new_title);
    }
}
void VinylDatabase::add_electronic_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<ElectronicDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(electronic_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Electronic", new_title);
    }
}
void VinylDatabase::add_classical_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<ClassicalDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(classical_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Classical", new_title);
    }
}
void VinylDatabase::add_hip_hop_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<HipHopDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(hip_hop_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Hip Hop", new_title);
    }
}
void VinylDatabase::add_reggae_disc(Author new_author, std::string new_title, int base_price, uint amount)
{
    std::shared_ptr<Disc> added_disc = std::make_shared<ReggaeDisc>(new_author, new_title, base_price);
    if (!(is_disc_in_database(reggae_music, new_title)))
    {
        shop_stock.insert({added_disc, amount});
    }
    else
    {
        throw DuplicateDiscError("Reggae", new_title);
    }
}

bool VinylDatabase::is_disc_in_database(Disc &disc) const
{
    for (const auto &disc_ptr : shop_stock)
    {
        if (disc_ptr.first->get_title() == disc.get_title() && disc_ptr.first->get_type_id() == disc.get_type_id())
        {
            return true;
        }
    }
    return false;
}

bool VinylDatabase::is_disc_in_database(MusicCategories category, std::string title) const
{
    for (const auto &disc_ptr : shop_stock)
    {
        if (disc_ptr.first->get_title() == title && disc_ptr.first->get_type_id() == category)
        {
            return true;
        }
    }
    return false;
}

auto VinylDatabase::get_disc_record(MusicCategories category, std::string title) const
{
    for (const auto &disc_ptr : shop_stock)
    {
        if (disc_ptr.first->get_title() == title && disc_ptr.first->get_type_id() == category)
        {
            return disc_ptr;
        }
    }
    throw DiscNotFoundError(title);
}

uint VinylDatabase::get_amount_of_disc(Disc &disc) const
{
    auto disc_data = get_disc_record(disc.get_type_id(), disc.get_title());

    return disc_data.second;
}

bool VinylDatabase::is_disc_in_stock(Disc &disc) const
{
    auto disc_record = get_disc_record(disc.get_type_id(), disc.get_title());
    return (disc_record.second > 0);
}

bool VinylDatabase::is_disc_in_stock(MusicCategories category, std::string title) const
{
    auto disc_record = get_disc_record(category, title);
    return (disc_record.second > 0);
}

Disc &VinylDatabase::get_disc(MusicCategories genre_id, std::string disc_name)
{
    auto disc_data = get_disc_record(genre_id, disc_name);

    return *(disc_data.first);
}

void VinylDatabase::decremente_amount(Disc &disc)
{
    for (const auto &disc_ptr : shop_stock)
    {
        if (disc_ptr.first->get_title() == disc.get_title() && disc_ptr.first->get_type_id() == disc.get_type_id())
        {
            --shop_stock.at(disc_ptr.first);
            return;
        }
    }
    throw DiscNotFoundError(disc.get_title());
}

DiscStatus VinylDatabase::sell_disc(Disc &disc_to_buy)
{
    if (is_disc_in_database(disc_to_buy))
    {
        uint amount = get_amount_of_disc(disc_to_buy);

        if (amount <= 0)
        {
            return avalible_to_order;
        }
        else
        {
            decremente_amount(disc_to_buy);
            return avalible_to_buy;
        }
    }
    else
    {
        throw DiscNotFoundError(disc_to_buy.get_title());
    }
}

std::shared_ptr<Disc> VinylDatabase::get_random_disc_from_database() const
{
    auto iterator = shop_stock.begin();
    int index = randint(shop_stock.size() - 1);
    std::advance(iterator, index);
    auto disc = iterator->first;
    return disc;
}

DiscStatus VinylDatabase::sell_disc(MusicCategories category, std::string title)
{
    if (is_disc_in_database(category, title))
    {
        Disc &disc_to_buy = get_disc(category, title);
        uint amount = get_amount_of_disc(disc_to_buy);

        if (amount <= 0)
        {
            return avalible_to_order;
        }
        else
        {
            decremente_amount(disc_to_buy);
            return avalible_to_buy;
        }
    }
    else
    {
        throw DiscNotFoundError(title);
    }
}

std::ostream &operator<<(std::ostream &os, VinylDatabase &database)
{
    for (const auto &disc_record : database.get_shop_stock())
    {
        os << disc_record.second << "x " << *disc_record.first << std::endl;
    }
    return os;
}
