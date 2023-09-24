#include "passThru.h"

ScreenComponent* passthru_send;
ScreenComponent* passthru_receive;
ScreenComponent* passthru_identifiers;

void passthru_init(Process* process) {
    passthru_send =         new ScreenComponent(2,0, 14,1);
    passthru_receive =      new ScreenComponent(2,1,14,1);
    passthru_identifiers =  new ScreenComponent(0,0, 1,2);
    passthru_identifiers->setText("><");

    Serial1.begin(115200);
}

void passthru_tick(unsigned long millis) {
    if (Serial1.available()) {
        char buffer[4];
        snprintf(buffer, 4, "%3d", Serial1.read());
        passthru_receive->scrollText(4);
        for (uint8_t i = 0; i < 3; i++) {
            passthru_receive->setCharAt(i, buffer[i]);
        }
        screen_keep_alive();
    }
}

bool passthru_exit(unsigned long millis) {
    delete passthru_send;
    delete passthru_receive;
    delete passthru_identifiers;
    Serial1.end();
    
    return true;
}

void passthru_input(uint8_t value) {
    Serial1.write(value);
    char buffer[4];
    snprintf(buffer, 4, "%3d", value);
    passthru_send->scrollText(4);
    for (uint8_t i = 0; i < 3; i++) {
        passthru_send->setCharAt(i, buffer[i]);
    }
    screen_keep_alive();
}

void passthru_render(bool is_rendering) {
    if (is_rendering) {
        screen_add_component(passthru_identifiers);
        screen_add_component(passthru_send);
        screen_add_component(passthru_receive);
    }
}