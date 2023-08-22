#include <Arduino.h>
#include "utils/supervisor.h"
#include "utils/screen.h"
#include "utils/power.h"
#include "processes/processes.h"
#include "utils/inputs.h"
#include "processes/home.h"

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
  supervisor_init(1);

  runProcess(
    generateProcess(
      1,
      screen_init,
      screen_tick,
      screen_exit
    )
  );

  // ScreenComponent* text = new ScreenComponent(0,0, 8,1);
  // text->setText("TEST!");
  // screen_add_component(text);

  runProcess(
    generateProcess(
      1,
      power_init,
      power_tick,
      power_exit
    )
  );

  // generateProcess(
  //   10,
  //   process_monitor_init,
  //   process_monitor_tick,
  //   process_monitor_exit,
  //   process_monitor_input,
  //   process_monitor_render,
  //   NULL
  // );

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
      1,
      process_monitor_init,
      process_monitor_tick,
      process_monitor_exit,
      process_monitor_input,
      process_monitor_render
    )
  );
  home_add_item(
    "other process",
    dummyProcessInfo
  );
  
  // runProcessWithoutDealloc(
  //   dummyProcessInfo
  // );
  // runProcessWithoutDealloc(
  //   dummyProcessInfo
  // );
  // runProcess(
  //   dummyProcessInfo
  // );

  power_init_pin(0, 1000, false);
}

unsigned long offset = 2000;
int stage = 0;
void loop() {
  supervisor_tick(millis());
  if (Serial.available()) {
    switch (Serial.read()) {
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
      case 13: // enter
        supervisor_input(INPUTS_SELECT);
        break;
      case 'q':
        supervisor_input(INPUTS_ESCAPE);
        break;
    }
  }

  delay(10);
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