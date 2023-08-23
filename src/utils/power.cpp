#include "power.h"

uint16_t power_data_timeouts[POWER_PINS]; // stores what data_timers counts down from
uint16_t power_data_timers[POWER_PINS]; // counts down to 0, at which point pin *CAN* be changed
uint8_t power_data_use[POWER_PINS]; // amount of processes using this pin

uint8_t* power_flags = create_flags(POWER_PINS); // if a pin is set to be turned on/off
uint8_t* power_flags_buffer = create_flags(POWER_PINS); // if a pins is currently on/off
uint8_t* power_is_inverted = create_flags(POWER_PINS);
uint8_t* power_is_locked = create_flags(POWER_PINS);

uint8_t power_flags_length;
unsigned long power_old_millis;
bool power_is_new_setting = true; // auto-init power pins
bool power_is_new_power;

void power_init(Process* process) {
    pinMode(POWER_SER_PIN, OUTPUT);
    pinMode(POWER_RCLK_PIN, OUTPUT);
    pinMode(POWER_SRCLK_PIN, OUTPUT);
    digitalWrite(POWER_RCLK_PIN, HIGH);

    power_flags_length = get_flags_length(POWER_PINS);

    // reset data
    // for (uint8_t i = 0; i < POWER_PINS; i++) {
    //     power_data_timeouts[i] = 0;
    //     power_data_use[i] = 0;
    // }

    power_old_millis = 0;
}

void power_tick(unsigned long millis) {
    uint16_t delta_time = millis - power_old_millis; // >= 0
    power_old_millis = millis;

    bool was_change = false;
    for (uint8_t i = 0; i < power_flags_length*8; i++) {
        if (power_data_timers[i]) {
            power_data_timers[i] = (power_data_timers[i] > delta_time) ? power_data_timers[i] - delta_time : 0; // change cannot be acted on
        }
        if (get_flag(power_flags, i) != get_flag(power_flags_buffer, i)) { // some change occurred
            if (power_data_timers[i] == 0) { // change can be acted on
                power_is_new_power = true;
                set_flag(power_flags_buffer, i, get_flag(power_flags,i));
                power_data_timers[i] = power_data_timeouts[i];
                was_change = true;
            }
        }
    }

    if (was_change || power_is_new_setting) { // shift out data
        for (uint8_t i = 0; i < power_flags_length; i++) {
            shiftOut(
                POWER_SER_PIN,
                POWER_SRCLK_PIN,
                LSBFIRST,
                power_flags_buffer[i] ^ power_is_inverted[i]
            );
        }
        digitalWrite(POWER_RCLK_PIN, LOW);
        digitalWrite(POWER_RCLK_PIN, HIGH);
        
        power_is_new_setting = false;
    }
}

bool power_exit(unsigned long millis) {
    // destroy_flags(power_flags);
    // destroy_flags(power_flags_buffer);
    // destroy_flags(power_is_inverted);
}

void power_set(size_t pin, bool enabled) { // not currently switching, change can occur
    if (enabled == (get_flag(power_flags, pin))) return; // no difference, so no point in changing anything
    if (enabled != (get_flag(power_flags_buffer, pin))) {
        set_flag(power_flags, pin, enabled);
    }
    else { // currently waiting for switch; disable that timer, as it is no longer required
        set_flag(power_flags, pin, enabled);
        power_data_timers[pin] = 0;
    }
}

void power_init_pin(size_t pin, uint16_t timeout, bool activeLow) {
    if (pin >= POWER_PINS) return;

    // set_flag(power_flags, pin, activeLow); // ensure all pins are off by default
    set_flag(power_is_inverted, pin, activeLow);
    power_data_timeouts[pin] = timeout;
    power_is_new_setting = true;
}

void power_use(size_t pin) {
    power_is_new_power = true;
    power_data_use[pin]++;
    
    if (!get_flag(power_is_locked, pin)) power_set(pin, true);
}

void power_release(size_t pin) {
    power_is_new_power = true;
    if (power_data_use[pin] == 0) return;
    power_data_use[pin]--;
    if (power_data_use[pin] == 0) power_set(pin, false);
}

void power_lock(size_t pin) {
    set_flag(power_is_locked, pin, true);
    power_set(pin, false);
}

void power_unlock(size_t pin) {
    set_flag(power_is_locked, pin, false);
    power_set(pin, power_data_use[pin] != 0);
}

bool power_is_enabled(size_t pin) {
    return get_flag(power_flags_buffer, pin);
}

uint8_t power_uses(size_t pin) {
    return power_data_use[pin];
}

bool power_is_new() {
    bool temp = power_is_new_power;
    power_is_new_power = false;
    return temp;
}