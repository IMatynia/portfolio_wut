#include "ingreedients_buffer.h"

std::ostream &operator<<(std::ostream &os, const Data &data)
{
    os << data.amount << "g " << data.ingredient_name << " (B_" << data.producer_id << "_" << data.batch << ")";
    return os;
}

IngredientBuffer::IngredientBuffer() : full(), empty(), Monitor() {
    this->top = 0;
}

void IngredientBuffer::put_ingredient(Data ingredient) {
    this->enter();
    if (this->top == BUFFER_SIZE)
        this->wait(full);

    this->data[this->top++] = ingredient;

    if (this->top == 1)
        this->signal(empty);
    this->leave();
}

Data IngredientBuffer::get_ingredient() {
    Data ingredient;
    this->enter();
    if (this->top == 0)
        this->wait(empty);

    ingredient = this->data[--this->top];

    if (this->top == BUFFER_SIZE - 1)
        this->signal(full);
    this->leave();
    return ingredient;
}
