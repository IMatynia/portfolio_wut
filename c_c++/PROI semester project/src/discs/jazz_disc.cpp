#include"jazz_disc.h"


    JazzDisc::JazzDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories JazzDisc::type_id() const
    {
        return jazz_music;
    }
    void JazzDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Jazz" << ", Total price: " << get_total_price_str();
    }
    int JazzDisc::get_total_price() const
    {
        return base_price * 1.45;
    }