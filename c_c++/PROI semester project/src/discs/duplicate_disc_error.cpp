#include"duplicate_disc_error.h"

DuplicateDiscError::DuplicateDiscError(std::string genre, std::string name): std::invalid_argument("Error: There already is a " + genre + " Disc with name " + name){}