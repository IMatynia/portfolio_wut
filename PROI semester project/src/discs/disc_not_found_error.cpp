#include"disc_not_found_error.h"

DiscNotFoundError::DiscNotFoundError(std::string name): std::invalid_argument("Error: Cannot find Disc with name" + name){}