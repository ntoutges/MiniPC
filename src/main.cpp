#include <Arduino.h>
#include "utils/supervisor.h"
#include "utils/screen.h"
#include "utils/power.h"
#include "processes/processes.h"
#include "utils/inputs.h"
#include "processes/home.h"
#include "processes/powerManager.h"
#include "processes/passThru.h"
#include "processes/plotter.h"

// TODO: figure out why random text starts appearing on screen (likely to do with linked list or supervisor)

uint16_t dummy_id;

void dummy_init(Process* process);
void dummy_tick(unsigned long millis);
bool dummy_exit(unsigned long millis);
void dummy_input(uint8_t value);
void dummy_render(bool is_rendering);

ProcessInfo* dummyProcessInfo;
void setup() {
  Serial.begin(115200);
  Serial1.begin(38400);
  supervisor_init(1);

  runProcess(
    generateProcess(
      50,
      screen_init,
      screen_tick,
      screen_exit
    )
  );
  
  runProcess(
    generateProcess(
      100,
      power_init,
      power_tick,
      power_exit
    )
  );

  runProcess(
    generateProcess(
      65535,
      home_init,
      home_tick,
      home_exit,
      home_input,
      home_render
    )
  );

  dummyProcessInfo = generateProcess(
    1,
    dummy_init,
    dummy_tick,
    dummy_exit
    // dummy_input,
    // dummy_render
  );

  home_add_item(
    "Processes",
    generateProcess(
      100,
      process_monitor_init,
      process_monitor_tick,
      process_monitor_exit,
      process_monitor_input,
      process_monitor_render
    )
  );

  home_add_item(
    "Power Manager",
    generateProcess(
      50,
      power_manager_init,
      power_manager_tick,
      power_manager_exit,
      power_manager_input,
      power_manager_render
    )
  );

  home_add_item(
    "PassThru",
    generateProcess(
      10,
      passthru_init,
      passthru_tick,
      passthru_exit,
      passthru_input,
      passthru_render
    )
  );

  home_add_item(
    "Plotter",
    generateProcess(
      1,
      plotter_init,
      plotter_tick,
      plotter_exit,
      plotter_input,
      plotter_render
    )
  );

  home_add_item(
    "other process",
    dummyProcessInfo
  );

  power_init_pin(7, 1000, false);
  power_init_pin(6, 5000, false);

  // testing
  // power_init_pin(6, 1000, false);
  // power_use(6);
}

unsigned long offset = 2000;
int stage = 0;
void loop() {
  supervisor_tick(millis());
  if (Serial.available()) {
    uint8_t val = Serial.read();
    switch (val) {
      case 'w':
        supervisor_input(INPUTS_ARROW_UP);
        break;
      case 'a':
        supervisor_input(INPUTS_ARROW_LEFT);
        break;
      case 's':
        supervisor_input(INPUTS_ARROW_DOWN);
        break;
      case 'd':
        supervisor_input(INPUTS_ARROW_RIGHT);
        break;
      case 10: // enter
      case 32: // space also works for select
        supervisor_input(INPUTS_SELECT);
        break;
      case 13: // \r -> ignore this
        break;
      case 'q':
        supervisor_input(INPUTS_ESCAPE);
        break;
      default:
        supervisor_input(val);
        break;
    }
  }
}

void dummy_init(Process* process) { dummy_id = process->getId(); }
void dummy_tick(unsigned long millis) {
  // if (millis > 2000) {
  //   closeProcess(dummy_id);
  // }
}
bool dummy_exit(unsigned long millis) { return true; }
void dummy_input(uint8_t value) {}
void dummy_render(bool is_rendering) {}