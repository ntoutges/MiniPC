#include "processes.h"

MenuComponent* processes_active_processes;
SingleMenuComponent* processes_remove_process;
ScreenComponent* processes_active_processes_count;
uint16_t processes_id;

void process_monitor_init(Process* process) {
    processes_id = process->getId();
    processes_active_processes = new MenuComponent(0,0, 8,0,1,2);
    processes_remove_process = new SingleMenuComponent(8,0, 8,1, 5);
    processes_active_processes_count = new ScreenComponent(13,1, 3,1);
    processes_active_processes_count->setText("0");
    process_monitor_update_menu();

    processes_remove_process->setItem(0, "");
    processes_remove_process->setItem(1, " Remove?");
    processes_remove_process->setItem(2, "mRemove?");
    processes_remove_process->setItem(3, ":Remove?");
    processes_remove_process->setItem(4, " :root:");
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
    processes_remove_process->selectItem(0);
    switch (value) {
        case INPUTS_ARROW_UP:
            processes_active_processes->scrollMenuItem(-1);
            break;
        case INPUTS_ARROW_DOWN:
            processes_active_processes->scrollMenuItem(1);
            break;
        case INPUTS_SELECT: {
            uint16_t id = processes_active_processes->selectMenuItem();
            switch (screen_get_state()) {
                case 0: {
                    uint8_t menu_index = (id == processes_id) ? 2 : (id > MAX_ROOT_ID) ? 1 : 3;
                    processes_remove_process->selectItem(menu_index);
                    screen_set_state(1);
                    return;
                }
                case 1:
                    if (id > MAX_ROOT_ID) closeProcess(id);
                    else processes_remove_process->setText(" :root:");
                    break;
            }
            break;
        }
        case INPUTS_ESCAPE:
            if (screen_get_state() == 1) {
                prevent_default_exit();
                processes_remove_process->selectItem(4);
            }
            break;
    }
    screen_set_state(0);
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
        
        // before means prepend to start of list
        processes_active_processes->addMenuItemBefore(buffer, temp_process->getId());
        temp_process = get_process_at_index(i);
        count++;
    }
    
    snprintf(buffer, 4, "%03d", count);

    processes_active_processes->scrollMenuItem(old_line);
    processes_active_processes_count->setText(buffer);
}