#include"electronic_disc.h"


    ElectronicDisc::ElectronicDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories ElectronicDisc::type_id() const
    {
        return electronic_music;
    }
    void ElectronicDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Electronic" << ", Total price: " << get_total_price_str();
    }
    int ElectronicDisc::get_total_price() const
    {
        return base_price * 1.05;
    }