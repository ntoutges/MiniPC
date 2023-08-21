#include "flags.h"

uint8_t* create_flags(size_t count) {
    size_t bytes = get_flags_length(count);
    return (uint8_t*) calloc(bytes, 1); // create empty array (filled with 0s)
}

void set_flag(uint8_t* flags, size_t index, bool flagValue) {
    uint8_t super_index = index / 8;
    uint8_t sub_index = index % 8;
    
    if (flagValue) flags[super_index] |= 1 << sub_index;
    else flags[super_index] &= ~(1 << sub_index); // negate bit mask to turn off bit
}

bool get_flag(uint8_t* flags, size_t index) {
    uint8_t super_index = index / 8;
    uint8_t sub_index = index % 8;
    return (flags[super_index] >> sub_index) & 1;
}

void destroy_flags(uint8_t* flags) {
    free(flags);
}

size_t get_flags_length(size_t count) {
    return (count-1) / 8 + 1; // rounds up
}