#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include "../utils/supervisor.h"
#include "../utils/power.h"
#include <ScreenComponent.h>
#include "../utils/screen.h"
#include "../chars.h"
#include "../utils/inputs.h"

void power_manager_init(Process* process);
void power_manager_tick(unsigned long millis);
bool power_manager_exit(unsigned long millis);
void power_manager_input(uint8_t value);
void power_manager_render(bool is_rendering);
void power_manager_update_counts();

#endif