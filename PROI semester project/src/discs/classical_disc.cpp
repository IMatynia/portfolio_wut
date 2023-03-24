#include"classical_disc.h"


    ClassicalDisc::ClassicalDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories ClassicalDisc::type_id() const
    {
        return classical_music;
    }
    void ClassicalDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Classical" << ", Total price: " << get_total_price_str();
    }
    int ClassicalDisc::get_total_price() const
    {
        return base_price * 1.4;
    }