#include"metal_disc.h"


    MetalDisc::MetalDisc(Author new_author, std::string new_title, int base_price): Disc( new_author, new_title, base_price)
    {

    }
    MusicCategories MetalDisc::type_id() const
    {
        return metal_music;
    }
    void MetalDisc::get_description_to_print(std::ostream& os) const
    {
        os << "Title: " << title << ", Author: " << author.get_full_name() << ", Genre: Metal" << ", Total price: " << get_total_price_str();
    }
    int MetalDisc::get_total_price() const
    {
        return base_price * 1.1;
    }