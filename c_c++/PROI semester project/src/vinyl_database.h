#ifndef VINYL_DATABASE_H
#define VINYL_DATABASE_H

#include <string>
#include <memory>
#include <vector>
#include "discs/disc.h"
#include "discs/author.h"
#include "utilities.h"
#include <map>

#define MAX_DISC_AMOUNT 5

//enum whoch represents state of Disc: either is avalible to order or to buy
enum DiscStatus
{
    avalible_to_buy,
    avalible_to_order
};

using nlohmann::json;
/**
 * @brief Class which holds the entire database of Discs
 *
 */
class VinylDatabase
{
private:
    std::map<std::shared_ptr<Disc>, uint> shop_stock;
    auto get_disc_record(MusicCategories category, std::string title) const;
    void decremente_amount(Disc &disc);
    void add_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_country_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_rock_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_metal_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_jazz_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_electronic_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_classical_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_hip_hop_disc(Author new_author, std::string new_title, int base_price, uint amount);
    void add_reggae_disc(Author new_author, std::string new_title, int base_price, uint amount);

public:
    VinylDatabase(json &all_discs);
    VinylDatabase() = default;

    ///Chcck if disc exists in database.
    bool is_disc_in_database(MusicCategories category, std::string title) const;
    ///Chcck if disc exists in database.
    bool is_disc_in_database(Disc &disc) const;
    /**
     * @brief Get the entire information about current database.
     *
     * @return std::map<std::shared_ptr<Disc>, uint>&
     */
    std::map<std::shared_ptr<Disc>, uint> &get_shop_stock();

    //Check if there is at least one copy of Disc in database.
    bool is_disc_in_stock(Disc &disc) const;

    //Check if there is at least one copy of Disc in database.
    bool is_disc_in_stock(MusicCategories category, std::string title) const;

    /**
     * @brief Get the disc object by given parameters.
     *
     * @return Disc&
     */
    Disc &get_disc(MusicCategories genre_id, std::string disc_name);

    ///Get the amount of copies of Disc in database
    uint get_amount_of_disc(Disc &disc_name) const;

    /**
     * @brief Try to buy the disc and update the database
     *
     * @return DiscStatus whether was avalible to buy before this action or was only avalible to order
     */
    DiscStatus sell_disc(MusicCategories category, std::string title);

    /**
     * @brief Try to buy the disc and update the database
     *
     * @return DiscStatus whether was avalible to buy before this action or was only avalible to order
     */
    DiscStatus sell_disc(Disc &disc_to_buy);

    /**
     * @brief Get the pointer of randomly selected Disc from this database.
     *
     * @return std::shared_ptr<Disc>
     */
    std::shared_ptr<Disc> get_random_disc_from_database() const;
    friend std::ostream &operator<<(std::ostream &os, VinylDatabase &database);
};

/// Print detailed info about the entire database
std::ostream &operator<<(std::ostream &os, VinylDatabase &database);

#endif