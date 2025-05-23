#include "formulas.h"



int create_h2o(unsigned int quantity) {
    unsigned int need_h = 2 * quantity;
    unsigned int need_o = 1 * quantity;

    if (hydrogen >= need_h && oxygen >= need_o) {
        hydrogen -= need_h;
        oxygen -= need_o;
        return 1;
    }
    return 0;
}

int create_carbon_dioxide(unsigned int quantity) {
    unsigned int need_c = 1 * quantity;
    unsigned int need_o = 2 * quantity;

    if (carbon >= need_c && oxygen >= need_o) {
        carbon -= need_c;
        oxygen -= need_o;
        return 1;
    }
    return 0;
}

int create_alcohol(unsigned int quantity) {
    unsigned int need_c = 2 * quantity;
    unsigned int need_h = 6 * quantity;
    unsigned int need_o = 1 * quantity;

    if (carbon >= need_c && hydrogen >= need_h && oxygen >= need_o) {
        carbon -= need_c;
        hydrogen -= need_h;
        oxygen -= need_o;
        return 1;
    }
    return 0;
}

int create_glucose(unsigned int quantity) {
    unsigned int need_c = 6 * quantity;
    unsigned int need_h = 12 * quantity;
    unsigned int need_o = 6 * quantity;

    if (carbon >= need_c && hydrogen >= need_h && oxygen >= need_o) {
        carbon -= need_c;
        hydrogen -= need_h;
        oxygen -= need_o;
        return 1;
    }
    return 0;
}
