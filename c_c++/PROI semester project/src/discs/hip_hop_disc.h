#ifndef HIP_HOP_DISC_H
#define HIP_HOP_DISC_H

#include"disc.h"

class HipHopDisc: public Disc
    {
        private:
            MusicCategories type_id() const override;
            void get_description_to_print(std::ostream& os) const override;
        public:
            HipHopDisc(Author new_author, std::string new_title, int base_price);
            int get_total_price() const override;
    };

#endif