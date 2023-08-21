#ifndef POWER_H
#define POWER_H

#include "supervisor.h"
#include <flags.h>
#include <Arduino.h>

#define POWER_PINS 8

#define POWER_SER_PIN 22
#define POWER_RCLK_PIN 23
#define POWER_SRCLK_PIN 24

void power_init(Process* process);
void power_tick(unsigned long millis);
bool power_exit(unsigned long millis);

void power_init_pin(size_t pin, uint16_t timeout, bool activeLow);
void power_set(size_t pin, bool enabled); // DO NOT USE THIS DIRECTLY FROM ANOTHER FILE

void power_use(size_t pin);
void power_release(size_t pin);
bool power_is_enabled(size_t pin);
uint8_t power_uses(size_t pin);

#endif