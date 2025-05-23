#include "drinks.h"





unsigned int min3(unsigned int a, unsigned int b, unsigned int c) {
    unsigned int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}


int count_soft_drink() {
    return water / 3;
}
int count_vodka() {
    if (water >= 1 && carbon_dioxide >= 1 && glucose >= 1)
        return min3(water, carbon_dioxide, glucose);
    return 0;
}

int count_champagne() {
    if (water >= 1 && carbon_dioxide >= 1 && alcohol >= 1)
        return min3(water, carbon_dioxide, alcohol);
    return 0;
}

int make_soft_drink(unsigned int quantity) {
    if (water < quantity || carbon_dioxide < quantity || glucose < quantity)
        return 0;

    water -= quantity;
    carbon_dioxide -= quantity;
    glucose -= quantity;
    return 1;
}

int make_vodka(unsigned int quantity) {
    if (water < quantity || carbon_dioxide < quantity || alcohol < quantity)
        return 0;

    water -= quantity;
    carbon_dioxide -= quantity;
    alcohol -= quantity;
    return 1;
}

int make_champagne(unsigned int quantity) {
    if (water < quantity || carbon_dioxide < quantity || glucose < quantity || alcohol < quantity)
        return 0;

    water -= quantity;
    carbon_dioxide -= quantity;
    glucose -= quantity;
    alcohol -= quantity;
    return 1;
}

