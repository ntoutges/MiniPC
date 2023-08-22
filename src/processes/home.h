#ifndef HOME_H
#define HOME_H

#include "../utils/supervisor.h"
#include "../utils/screen.h"
#include <ScreenComponent.h>
#include "../chars.h"
#include "../utils/inputs.h"

void home_init(Process* process);
void home_tick(unsigned long millis);
bool home_exit(unsigned long millis);

void home_input(uint8_t value);
void home_render(bool is_rendering);

void home_add_item( char* name, ProcessInfo* process );
#endif