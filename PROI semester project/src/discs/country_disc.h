#ifndef COUNTRY_DISC_H
#define COUNTRY_DISC_H

#include"disc.h"

class CountryDisc: public Disc
    {
        private:
            MusicCategories type_id() const override;
            void get_description_to_print(std::ostream& os) const override;
        public:
            CountryDisc(Author new_author, std::string new_title, int base_price);
            int get_total_price() const override;
    };

#endif