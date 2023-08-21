#include "processes.h"

MenuComponent* processes_active_processes;
uint16_t processes_id;

void process_monitor_init(Process* process) {
     processes_id = process->getId();
}

void process_monitor_tick(unsigned long millis) {
    if (is_new_processes()) {
        process_monitor_update_menu();
    }
}

bool process_monitor_exit(unsigned long millis) {
    delete processes_active_processes;
    return true;
}

void process_monitor_input(uint8_t value) {
    if (!processes_active_processes) return;
    switch (value) {
        case INPUTS_ARROW_UP:
            processes_active_processes->scrollMenuItem(-1);
            break;
        case INPUTS_ARROW_DOWN:
            processes_active_processes->scrollMenuItem(1);
            break;
        // case INPUTS_ARROW_LEFT:
        //     break;
        // case INPUTS_ARROW_RIGHT:    
        //     break;
        // case INPUTS_SELECT:
        //     break;
        case INPUTS_ESCAPE:
            closeProcess(processes_id);
            break;
        default:
            return;
    }
    screen_keep_alive();
}

void process_monitor_render(bool is_rendering) {
    if (is_rendering) {
        processes_active_processes = new MenuComponent(0,0, 8,0,1,2);
        screen_create_char(0, chr_arrow_up);
        screen_create_char(1, chr_arrow_down);
        screen_create_char(2, chr_ellipses);
    }
    else {
        delete processes_active_processes;
        processes_active_processes = NULL;
    }
}

void process_monitor_update_menu() {
    if (!processes_active_processes) return;

    uint8_t old_line = processes_active_processes->getLine();

    processes_active_processes->clear();
    Process* temp_process = get_process_at_index(0);
    for (uint16_t i = 1; temp_process; i++) {
        char buffer[8];
        snprintf(buffer, 8, "0x%02x", temp_process->getId());
        
        processes_active_processes->addMenuItem(buffer, i);
        temp_process = get_process_at_index(i);
    }
    
    processes_active_processes->scrollMenuItem(old_line);
    screen_add_component(processes_active_processes);
}