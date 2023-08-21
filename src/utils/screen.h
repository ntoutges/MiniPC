#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <LinkedList.h>
#include <ScreenComponent.h>
#include "supervisor.h"
#include <LiquidCrystal.h>
#include "power.h"

#define SCREEN_POWER_PIN 7

#define SCREEN_RS 53
#define SCREEN_EN 52
#define SCREEN_DATA_4 51
#define SCREEN_DATA_5 50
#define SCREEN_DATA_6 49
#define SCREEN_DATA_7 48

#define SCREEN_TIMEOUT 10000 // 10 seconds of no keep-alive signal before turning off

void screen_init(Process* process);
void screen_tick(unsigned long millis);
bool screen_exit(unsigned long millis);

void screen_add_component(ScreenComponent* component);
void screen_clear();
void screen_create_char(uint8_t index, uint8_t character[8]);

bool screen_enable();
bool screen_disable();

void screen_keep_alive(); // if not called often enough, screen will turn off

#endif