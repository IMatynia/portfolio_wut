#include"reggae_disc.h"


    ReggaeDisc::ReggaeDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories ReggaeDisc::type_id() const
    {
        return reggae_music;
    }
    void ReggaeDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Reggae" << ", Total price: " << get_total_price_str();
    }
    int ReggaeDisc::get_total_price() const
    {
        return base_price * 1.05;
    }