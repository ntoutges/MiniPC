#ifndef PROCESSES_H
#define PROCESSES_H

#include "../utils/supervisor.h"
#include "../utils/screen.h"
#include "../chars.h"
#include "../utils/inputs.h"

void process_monitor_init(Process* process);
void process_monitor_tick(unsigned long millis);
bool process_monitor_exit(unsigned long millis);
void process_monitor_input(uint8_t value);
void process_monitor_render(bool init);

void process_monitor_update_menu();

#endif