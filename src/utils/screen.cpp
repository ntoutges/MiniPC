#include "screen.h"

LinkedList<ScreenComponent> screen_components;
LiquidCrystal screen_lcd(SCREEN_RS, SCREEN_EN, SCREEN_DATA_4,SCREEN_DATA_5,SCREEN_DATA_6,SCREEN_DATA_7);
unsigned long screen_timer;
unsigned long screen_last_millis;
bool screen_is_on;
bool screen_will_on;
bool screen_is_in_transition;
uint16_t screen_state;

uint8_t screen_custom_chars[8][8];

void screen_init(Process* process) {
  screen_enable();

  screen_lcd.begin(16,2);
  screen_lcd.print("PiCO");
  screen_keep_alive();
  screen_state = 0;
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

  if (!screen_is_on) return; // don't try to update the screen if it is turned off

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
  component->setHasChanged(true); // immediately render after being added
  screen_components.insertItem(component);
}

void screen_clear() {
  // repeatedly remove and deallocate first item
  while (!screen_components.isEmpty()) {
    screen_components.removeItem(0);
  }

  if (screen_is_on) screen_lcd.clear();
  screen_set_state(0); // reset state
}

void screen_clear_without_dealloc() {
  // repeatedly remove first item
  while (!screen_components.isEmpty()) {
    screen_components.removeItemWithoutDealloc(0);
  }
  if (screen_is_on) screen_lcd.clear();
  screen_set_state(0); // reset state
}

// bug: if screen turns off with custom character, custom characters are lost (volatile memory) -- these must be saved at some time
void screen_create_char(uint8_t index, uint8_t character[8]) {
  for (uint8_t i = 0; i < 8; i++) { // copy character into [screen_custom_chars] 
    screen_custom_chars[index][i] = character[i];
  }

  if (screen_is_on) screen_lcd.createChar(index, character);
}

bool screen_enable() {
  if (!screen_is_in_transition) power_use(SCREEN_POWER_PIN);
  screen_is_in_transition = true;
  if (!power_is_enabled(SCREEN_POWER_PIN)) return false; // waiting to turn on
  screen_is_in_transition = false;

  digitalWrite(SCREEN_RS, LOW);
  digitalWrite(SCREEN_EN, LOW);
  digitalWrite(SCREEN_DATA_4, LOW);
  digitalWrite(SCREEN_DATA_5, LOW);
  digitalWrite(SCREEN_DATA_6, LOW);
  digitalWrite(SCREEN_DATA_7, LOW);

  screen_lcd.begin(16,2);

  // copy custom characters from arduino into screen
  for (uint8_t i = 0; i < 8; i++) {
    screen_lcd.createChar(i, screen_custom_chars[i]);
  }

  return true; // screen enable finished
}

bool screen_disable() {
  if (!screen_is_in_transition) power_release(SCREEN_POWER_PIN);
  screen_is_in_transition = true;
  if (power_is_enabled(SCREEN_POWER_PIN)) return false; // waiting to turn off
  screen_is_in_transition = false;

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

void screen_set_state(uint16_t state) {
  screen_state = state;
}

uint16_t screen_get_state() {
  return screen_state;
}
