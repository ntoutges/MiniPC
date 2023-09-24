#ifndef PASSTHRU_H
#define PASSTHRU_H

#include "../utils/supervisor.h"
#include "../utils/screen.h"
#include <ScreenComponent.h>

void passthru_init(Process* process);
void passthru_tick(unsigned long millis);
bool passthru_exit(unsigned long millis);

void passthru_input(uint8_t value);
void passthru_render(bool is_rendering);

#endif