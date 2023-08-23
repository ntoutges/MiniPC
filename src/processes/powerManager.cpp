#include "powerManager.h"

TriStateSelectorComponent* power_manager_selector;
ScreenComponent* power_manager_name;

void power_manager_init(Process* process) {
    power_manager_selector = new TriStateSelectorComponent(0,0,8, 0,1,2, 3,4,5);
    power_manager_name = new ScreenComponent(8,1, 8,1);
    power_manager_update_counts();
}

void power_manager_tick(unsigned long millis) {
    if (power_is_new()) { power_manager_update_counts(); }
}

bool power_manager_exit(unsigned long millis) {
    free(power_manager_selector);
    free(power_manager_name);
    return true;
}

void power_manager_input(uint8_t value) {
    screen_keep_alive();
    
    bool wasChange = false;
    bool wasA = power_manager_selector->isStateA();
    bool wasB = power_manager_selector->isStateB();
    
    switch (value) {
        case INPUTS_ARROW_UP:
            wasChange = power_manager_selector->changeItemState(-1);
            break;
        case INPUTS_ARROW_DOWN:
            wasChange = power_manager_selector->changeItemState(1);
            break;
        case INPUTS_ARROW_LEFT:
            power_manager_selector->scrollPointer(-1);
            break;
        case INPUTS_ARROW_RIGHT:
            power_manager_selector->scrollPointer(1);
            break;
        case INPUTS_ESCAPE:
            break;
        case INPUTS_SELECT:
            break;
    }

    if (wasChange) {
        // _ -> A : use power
        // _ -> B : lock power
        // A -> _ : release power
        // B -> _ : unlock power

        uint8_t pin = power_manager_selector->getCursor();
        if (wasA) { // A -> _
            power_release(pin);
        }
        else if (wasB) { // B -> _
            power_unlock(pin);
        }
        else if (power_manager_selector->isStateA()) { // _ -> A
            power_use(pin);
        }
        else { // isStateB() -> true // _ -> B
            power_lock(pin);
        }
    }
}

void power_manager_render(bool is_rendering) {
    if (is_rendering) {
        screen_create_char(0, chr_selected);
        screen_create_char(1, chr_selected_a);
        screen_create_char(2, chr_selected_b);
        screen_create_char(3, chr_unselected);
        screen_create_char(4, chr_unselected_a);
        screen_create_char(5, chr_unselected_b);

        screen_add_component(power_manager_selector);
        screen_add_component(power_manager_name);
    }
}

void power_manager_update_counts() {
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t uses = power_uses(i);
        char text = (uses == 0 ) ? '/' : (uses <= 9) ? ('0' + uses) : 'X';
        power_manager_selector->setStateSymbol(i, text);
    }
}