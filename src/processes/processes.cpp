#include "processes.h"

MenuComponent* processes_active_processes;
ScreenComponent* processes_remove_process;
ScreenComponent* processes_active_processes_count;
uint16_t processes_id;

void process_monitor_init(Process* process) {
    processes_id = process->getId();
    processes_active_processes = new MenuComponent(0,0, 8,0,1,2);
    processes_remove_process = new ScreenComponent(8,0, 8,1);
    processes_active_processes_count = new ScreenComponent(13,1, 3,1);
    processes_active_processes_count->setText("0");
    process_monitor_update_menu();
}

void process_monitor_tick(unsigned long millis) {
    if (is_new_processes()) {
        process_monitor_update_menu();
    }
}

bool process_monitor_exit(unsigned long millis) {
    delete processes_active_processes;
    delete processes_active_processes_count;

    // signify to the rest of the program that this is not being rendered
    processes_active_processes = NULL;
    return true;
}

void process_monitor_input(uint8_t value) {
    screen_keep_alive();
    switch (value) {
        case INPUTS_ARROW_UP:
            processes_active_processes->scrollMenuItem(-1);
            screen_set_state(0);
            processes_remove_process->setText("");
            break;
        case INPUTS_ARROW_DOWN:
            processes_active_processes->scrollMenuItem(1);
            screen_set_state(0);
            processes_remove_process->setText("");
            break;
        case INPUTS_SELECT: {
            uint16_t id = processes_active_processes->selectMenuItem();
            switch (screen_get_state()) {
                case 0: {
                    char* text = (id == processes_id) ? "mRemove?" : ((id > MAX_ROOT_ID) ? " Remove?" : ":Remove");
                    processes_remove_process->setText(text);
                    screen_set_state(1);
                    break;
                }
                case 1:
                    screen_set_state(0);
                    if (id > MAX_ROOT_ID) {
                        closeProcess(id);
                        processes_remove_process->setText("");
                    }
                    else {
                        processes_remove_process->setText(" :root:");
                    }
                    break;
            }
            break;
        }
        case INPUTS_ESCAPE:
            if (screen_get_state() == 1) {
                prevent_default_exit();
                screen_set_state(0);
                processes_remove_process->setText("");
            }
            break;
    }
}

void process_monitor_render(bool is_rendering) {
    if (is_rendering) {
        screen_create_char(0, chr_arrow_up);
        screen_create_char(1, chr_arrow_down);
        screen_create_char(2, chr_ellipses);

        screen_add_component(processes_active_processes);
        screen_add_component(processes_remove_process);
        screen_add_component(processes_active_processes_count);
    }
    else {
        screen_clear_without_dealloc();
    }
}

void process_monitor_update_menu() {
    if (!processes_active_processes) return;

    uint8_t old_line = processes_active_processes->getLine();
    uint16_t count = 0;

    processes_active_processes->clear();
    Process* temp_process = get_process_at_index(0);
    char buffer[8];
    for (uint16_t i = 1; temp_process; i++) {
        snprintf(buffer, 8, "0x%02x", temp_process->getId());
        
        processes_active_processes->addMenuItem(buffer, temp_process->getId());
        temp_process = get_process_at_index(i);
        count++;
    }
    
    snprintf(buffer, 4, "%03d", count);

    processes_active_processes->scrollMenuItem(old_line);
    processes_active_processes_count->setText(buffer);
}