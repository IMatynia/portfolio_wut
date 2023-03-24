#include"country_disc.h"


    CountryDisc::CountryDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories CountryDisc::type_id() const
    {
        return country_music;
    }
    void CountryDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Country" << ", Total price: " << get_total_price_str();
    }
    int CountryDisc::get_total_price() const
    {
        return base_price * 1.5;
    }