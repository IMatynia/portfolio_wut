#include"rock_disc.h"


    RockDisc::RockDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories RockDisc::type_id() const
    {
        return rock_music;
    }
    void RockDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Rock" << ", Total price: " << get_total_price_str();
    }
    int RockDisc::get_total_price() const
    {
        return base_price * 1.2;
    }