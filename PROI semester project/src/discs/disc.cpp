#include "disc.h"
#include "author.h"
#include "negative_price_error.h"
#include "empty_title_error.h"
#include <string>
#include <iostream>

std::map<std::string, MusicCategories> Disc::str_to_genre{
    {"unknown", none_genre},
    {"country", country_music},
    {"rock", rock_music},
    {"metal", metal_music},
    {"jazz", jazz_music},
    {"electronic", electronic_music},
    {"classical", classical_music},
    {"hip_hop", hip_hop_music},
    {"reggae", reggae_music}};

Disc::Disc(Author new_author, std::string new_title, int base_price = 0) : author(new_author), title(new_title), base_price(base_price)
{
        if (title == "")
        {
                throw EmptyTitleError();
        }
        if (base_price < 0)
        {
                throw NegavitePriceError();
        }
}

Disc::Disc() {}

MusicCategories Disc::type_id() const
{
        return none_genre;
}

MusicCategories Disc::get_type_id() const
{
        return type_id();
}

void Disc::get_description_to_print(std::ostream &os) const
{
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Total price: " << get_total_price_str();
}

int Disc::get_total_price() const
{
        return base_price;
}

Disc::~Disc()
{
}

Author Disc::get_author() const
{
        return author;
}

void Disc::set_author(Author new_author)
{
        author = new_author;
}

int Disc::get_base_price() const
{
        return base_price;
}

void Disc::set_base_price(int new_value)
{
        if (new_value < 0)
        {
                throw NegavitePriceError();
        }
        base_price = new_value;
}

std::string Disc::get_title()
{
        return title;
}

std::string Disc::get_total_price_str() const
{
        int main_currency = get_total_price() / 100;
        int minor_currency = get_total_price() % 100;
        std::string add_zero = minor_currency == 0 ? "0" : "";
        return std::to_string(main_currency) + "," + std::to_string(minor_currency) + add_zero;
}

bool Disc::operator==(const Disc &second) const
{
        return (type_id() == second.type_id() && title == second.title);
}
bool Disc::operator!=(const Disc &second) const
{
        return !(*this == second);
}
bool Disc::operator>(const Disc &second) const
{
        if (type_id() != second.type_id())
        {
                return type_id() > second.type_id();
        }
        else
        {
                int comp = title.compare(second.title);
                if (comp >= 0)
                {
                        return true;
                }

                return false;
        }
}
bool Disc::operator<(const Disc &second) const
{
        return (second > *this);
}
bool Disc::operator>=(const Disc &second) const
{
        return !(*this < second);
}
bool Disc::operator<=(const Disc &second) const
{
        return !(*this > second);
}

std::ostream &operator<<(std::ostream &os, const Disc &disc)
{
        disc.get_description_to_print(os);
        return os;
}