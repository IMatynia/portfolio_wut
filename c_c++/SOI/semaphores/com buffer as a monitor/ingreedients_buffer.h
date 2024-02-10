#ifndef LAB4_INGREDIENTS_BUFFER_H
#define LAB4_INGREDIENTS_BUFFER_H

#include "monitor.h"
#include <iostream>

#define BUFFER_SIZE 10

struct Data_s {
    int batch;
    int producer_id;
    float amount;
    const char *ingredient_name;
};
typedef struct Data_s Data;

std::ostream &operator<<(std::ostream &os, const Data &data);

class IngredientBuffer : private Monitor {
public:
    IngredientBuffer();
    void put_ingredient(Data ingredient);
    Data get_ingredient();
private:
    Condition full, empty;
    Data data[BUFFER_SIZE]{};
    size_t top;
};


#endif //LAB4_INGREDIENTS_BUFFER_H
