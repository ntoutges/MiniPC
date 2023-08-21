#include <Arduino.h>
#include "utils/supervisor.h"
#include "utils/screen.h"
#include "utils/power.h"
#include "processes/processes.h"
#include "utils/inputs.h"

TriStateSelectorComponent* selector = new TriStateSelectorComponent(
  0,0,
  8,
  'A','B','C','a','b','c'
);

void setup() {
  Serial.begin(115200);
  supervisor_init(1);

  generateProcess(
    10,
    screen_init,
    screen_tick,
    screen_exit
  );

  ScreenComponent* text = new ScreenComponent(0,0, 8,1);
  text->setText("TEST!");
  screen_add_component(text);

  generateProcess(
    10,
    power_init,
    power_tick,
    power_exit
  );

  generateProcess(
    10,
    process_monitor_init,
    process_monitor_tick,
    process_monitor_exit,
    process_monitor_input,
    process_monitor_render,
    NULL
  );

  power_init_pin(0, 1000, false);
}

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
    }
  }

  delay(10);
}
