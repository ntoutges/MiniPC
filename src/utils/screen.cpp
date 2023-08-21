#include "screen.h"

LinkedList screen_components;
LiquidCrystal screen_lcd(SCREEN_RS, SCREEN_EN, SCREEN_DATA_4,SCREEN_DATA_5,SCREEN_DATA_6,SCREEN_DATA_7);
unsigned long screen_timer;
unsigned long screen_last_millis;
bool screen_is_on;
bool screen_will_on;

void screen_init(Process* process) {
  screen_lcd.begin(16,2);
  screen_lcd.print("PiCO");

  screen_enable();
  screen_keep_alive();
}

void screen_tick(unsigned long millis) {
  screen_last_millis = millis;
  if (millis > screen_timer) {
    screen_will_on = false;
  }

  bool doFillScreen = false;
  if (screen_is_on != screen_will_on) {
    if (screen_is_on) { // turn screen off
      if (screen_disable()) screen_is_on = false;
      return;
    }
    else { // turn screen on
      if (screen_enable()) {
        screen_is_on = true;
        doFillScreen = true;
      }
      else return;
    }
  }

  screen_components.loopInit();
  while (!screen_components.isLoopDone()) {
    ScreenComponent* component = (ScreenComponent*) screen_components.loopNext();
    if (component->hasChanged() || doFillScreen) {
      uint8_t xOff = component->getX();
      uint8_t yOff = component->getY();
      uint8_t height = component->height();
      uint8_t width = component->width();
      for (uint8_t y = 0; y < height; y++) {
        screen_lcd.setCursor(xOff, yOff+y);
        for (uint8_t x = 0; x < width; x++) {
          screen_lcd.print(component->getCharAt(xOff + x, yOff + y));
        }
      }
      component->setHasChanged(false);
    }
  }
}

bool screen_exit(unsigned long millis) {
  return true;
}

void screen_add_component(ScreenComponent* component) {
  screen_components.insertItem(component);
}

void screen_clear() {
  // repeatedly remove first item
  while (!screen_components.isEmpty()) {
    screen_components.removeItem(0);
  }
  screen_lcd.clear();
}

// bug: if screen turns off with custom character, custom characters are lost (volatile memory) -- these must be saved at some time
void screen_create_char(uint8_t index, uint8_t character[8]) {
  screen_lcd.createChar(index, character);
}

bool screen_enable() {
  power_use(SCREEN_POWER_PIN);
  if (!power_is_enabled(SCREEN_POWER_PIN)) return false; // waiting to turn on

  digitalWrite(SCREEN_RS, LOW);
  digitalWrite(SCREEN_EN, LOW);
  digitalWrite(SCREEN_DATA_4, LOW);
  digitalWrite(SCREEN_DATA_5, LOW);
  digitalWrite(SCREEN_DATA_6, LOW);
  digitalWrite(SCREEN_DATA_7, LOW);

  screen_lcd.begin(16,2);

  return true; // screen enable finished
}

bool screen_disable() {
  power_release(SCREEN_POWER_PIN);
  if (power_is_enabled(SCREEN_POWER_PIN)) return false; // waiting to turn off

  digitalWrite(SCREEN_RS, HIGH);
  digitalWrite(SCREEN_EN, HIGH);
  digitalWrite(SCREEN_DATA_4, HIGH);
  digitalWrite(SCREEN_DATA_5, HIGH);
  digitalWrite(SCREEN_DATA_6, HIGH);
  digitalWrite(SCREEN_DATA_7, HIGH);

  return true; // screen disable finished
}

void screen_keep_alive() {
  screen_timer = screen_last_millis + SCREEN_TIMEOUT;
  screen_will_on = true;
}
