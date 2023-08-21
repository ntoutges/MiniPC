#ifndef FLAGS_H
#define FLAGS_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t* create_flags(size_t count);
void set_flag(uint8_t* flags, size_t index, bool flagValue);
bool get_flag(uint8_t* flags, size_t index);
void destroy_flags(uint8_t* flags);
size_t get_flags_length(size_t count);

#endif