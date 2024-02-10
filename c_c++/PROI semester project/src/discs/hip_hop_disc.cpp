#include"hip_hop_disc.h"


    HipHopDisc::HipHopDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories HipHopDisc::type_id() const
    {
        return hip_hop_music;
    }
    void HipHopDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Hip Hop" << ", Total price: " << get_total_price_str();
    }
    int HipHopDisc::get_total_price() const
    {
        return base_price * 1.1;
    }