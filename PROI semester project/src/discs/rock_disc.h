#ifndef ROCK_DISC_H
#define ROCK_DISC_H

#include"disc.h"

class RockDisc: public Disc
    {
        private:
            MusicCategories type_id() const override;
            void get_description_to_print(std::ostream& os) const override;
        public:
            RockDisc(Author new_author, std::string new_title, int base_price);
            int get_total_price() const override;
            ~RockDisc() = default;
    };

#endif