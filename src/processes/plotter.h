#ifndef PLOTTER_H
#define POTTER_H

#include "../utils/supervisor.h"
#include "../utils/power.h"
#include "../utils/screen.h"
#include "../utils/inputs.h"
#include "../chars.h"

void plotter_init(Process* process);
void plotter_tick(unsigned long millis);
bool plotter_exit(unsigned long millis);
void plotter_input(uint8_t value);
void plotter_render(bool is_rendering);

void plotter_update_selector();

void plotter_set_graph_count(uint8_t plots);
void plotter_send(uint8_t plot, float data);

void plotter_clear();
void plotter_add();
void plotter_pop();

#endif