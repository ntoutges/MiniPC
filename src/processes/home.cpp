#include "home.h"

MenuComponent* home_menu = new MenuComponent(0,0, 16, 0,1,2);
LinkedList<ProcessInfo>* home_processes = new LinkedList<ProcessInfo>();
uint16_t home_id;

void home_init(Process* process) {
    home_id = process->getId();
}

void home_tick(unsigned long millis) {}

bool home_exit(unsigned long millis) {
    home_menu->clear();

    // clear [home_processes]
    home_processes->loopInit();
    while (!home_processes->isLoopDone()) {
        home_processes->removeItem(0);
    }
}

void home_input(uint8_t value) {
    screen_keep_alive();
    switch (value) {
        case INPUTS_ARROW_UP:
            home_menu->scrollMenuItem(-1);
            break;
        case INPUTS_ARROW_DOWN:
            home_menu->scrollMenuItem(1);
            break;
        // case INPUTS_ARROW_LEFT:
        //     break;
        // case INPUTS_ARROW_RIGHT:    
        //     break;
        case INPUTS_SELECT:
            runProcessWithoutDealloc( // allows process to be reused
                (ProcessInfo*) home_processes->getItem(
                    home_menu->selectMenuItem()
                )
            );
            break;
        case INPUTS_ESCAPE: // unpossible to close home
            prevent_default_exit();
            break;
    }
}

void home_render(bool is_rendering) {
    if (is_rendering) {
        screen_create_char(0, chr_arrow_up);
        screen_create_char(1, chr_arrow_down);
        screen_create_char(2, chr_ellipses);

        screen_add_component(home_menu);
    }
    else {
        screen_clear_without_dealloc();
    }
}

void home_add_item( char* name, ProcessInfo* process ) {
    // set value to current position in process
    home_menu->addMenuItem(name, home_processes->length());
    home_processes->insertItem(process);
}