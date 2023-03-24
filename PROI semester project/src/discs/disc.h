#ifndef DISC_H
#define DISC_H

#include "author.h"
#include <string>
#include <iostream>
#include <map>

/// enum representing music category of disc
enum MusicCategories
{
    none_genre,
    country_music,
    rock_music,
    metal_music,
    jazz_music,
    electronic_music,
    classical_music,
    hip_hop_music,
    reggae_music,
};

/// enum representing action which customer can make
enum DiscAction
{
    ask,
    order,
    buy
};
/**
 * @brief Class representing disc from database
 *
 */
class Disc
{

protected:
    Author author;
    std::string title;
    int base_price;
    /**
     * @brief get MusicCategory of disc
     *
     * @return MusicCategories
     */
    virtual MusicCategories type_id() const;
    /**
     * @brief Get price of this disc, written as string f.e "12,00"
     *
     * @return std::string
     */
    std::string get_total_price_str() const;

public:
    /**
     * @brief Map for gettong MusicCategory out of string, f.e "rock" -> MusicCategory Rock
     *
     */
    static std::map<std::string, MusicCategories> str_to_genre;

    /**
     * @brief Construct a new Disc object
     *
     * @param new_author Author (object) of Disc
     * @param new_title Ttile of Disc
     * @param base_price Base price (without margin) of Disc
     */
    Disc(Author new_author, std::string new_title, int base_price);
    Disc();
    /**
     * @brief Get the description of Disc object, which will used in operator<<
     *
     * @param os osstream to print this description
     */
    virtual void get_description_to_print(std::ostream &os) const;
    /**
     * @brief Get the total price (with margin) of Disc
     *
     * @return int
     */
    virtual int get_total_price() const;

    /**
     * @brief Get the author object
     *
     * @return Author
     */
    Author get_author() const;
    /**
     * @brief Set the new author object
     *
     * @param new_author
     */
    void set_author(Author new_author);

    /**
     * @brief Get the base_price of Disc (without margin)
     *
     * @return int
     */
    int get_base_price() const;
    /**
     * @brief Set the base_price of Disc
     *
     * @param new_value
     */
    void set_base_price(int new_value);

    /**
     * @brief Get the title of Disc
     *
     * @return std::string
     */
    std::string get_title();

    /**
     * @brief Get the MusicCategories of Disc
     *
     * @return MusicCategories
     */
    MusicCategories get_type_id() const;

    /**
     * @brief Returns true if titles and MusicCategories of two Dsics are the same.
     *
     */
    bool operator==(const Disc &second) const;

    /**
     * @brief Returns true if titles or MusicCategories of two Dsics are diffrent.
     *
     */
    bool operator!=(const Disc &second) const;

    /**
     * @brief Returns true if total price of first Disc is greater than the second one.
     *
     */
    bool operator>(const Disc &second) const;

    /**
     * @brief Returns true if total price of first Disc is less than the second one.
     *
     */
    bool operator<(const Disc &second) const;

    /**
     * @brief Returns true if total price of first Disc is greater or equal than the second one.
     *
     */
    bool operator>=(const Disc &second) const;


    /**
     * @brief Returns true if total price of first Disc is less or equal than the second one.
     *
     */
    bool operator<=(const Disc &second) const;

    virtual ~Disc();

    friend std::ostream &operator<<(std::ostream &os, const Disc &disc);
};

std::ostream &operator<<(std::ostream &os, const Disc &disc);

#endif