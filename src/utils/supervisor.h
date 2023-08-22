#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <stdint.h>
#include <stdlib.h>
#include "LinkedList.h"
#include "inputs.h"

#define MAX_ROOT_ID 2

class Process {
private:
    uint16_t m_process_id;
    uint16_t m_delay;
    uint16_t m_delay_tracker;

    bool m_is_initialized;

    void (*m_init)(Process*);
    void (*m_tick)(unsigned long millis);
    bool (*m_exit)(unsigned long millis);
    void (*m_input)(uint8_t value);
    void (*m_render)(bool is_rendering);

public:
    Process(
        uint16_t process_id,
        uint16_t delay,
        void (*init)(Process*),
        void (*tick)(unsigned long millis),
        bool (*exit)(unsigned long millis)
    );
    Process(
        uint16_t process_id,
        uint16_t delay,
        void (*init)(Process*),
        void (*tick)(unsigned long millis),
        bool (*exit)(unsigned long millis),
        void (*input)(uint8_t value),
        void (*render)(bool is_rendering) // this is only called when rendering status is changed
    );

    uint16_t getId();
    bool isTickable();
    bool hasGUI();

    void init(Process* process);
    void tick(unsigned long millis);
    void render(bool is_rendering);
    bool exit(unsigned long millis);
    void input(uint8_t value);

private:
    void init(
        uint16_t process_id,
        uint16_t delay,
        void (*init)(Process*),
        void (*tick)(unsigned long millis),
        bool (*exit)(unsigned long millis),
        void (*input)(uint8_t value),
        void (*render)(bool is_rendering)
    );
};

struct ProcessInfo {
    uint16_t delay;
    void (*init)(Process*);
    void (*tick)(unsigned long millis);
    bool (*exit)(unsigned long millis);
    void (*input)(uint8_t value);
    void (*render)(bool is_rendering);
};

void supervisor_tick(unsigned long millis);
void supervisor_init(uint16_t ms_per_tick);
void supervisor_input(uint8_t value);


// generateProcess creates the information object, for use later
ProcessInfo* generateProcess(
    uint16_t delay,
    void (*init)(Process*),
    void (*tick)(unsigned long millis),
    bool (*exit)(unsigned long millis)
);

ProcessInfo* generateProcess(
    uint16_t delay,
    void (*init)(Process*),
    void (*tick)(unsigned long millis),
    bool (*exit)(unsigned long millis),
    void (*input)(uint8_t value),
    void (*render)(bool is_rendering)
);

// runProcess actually puts process into queue to run, and automatically frees memory block of [processInfo]
Process* runProcess(ProcessInfo* processInfo);
Process* runProcessWithoutDealloc(ProcessInfo* processInfo); // (this doesn't do any freeing)

void closeProcess(uint16_t id);
void _removeProcess(uint16_t id);
Process* _getLastGUIProcess();

void setActiveProcess(uint16_t id);

uint16_t generate_process_id();
uint16_t get_process_index(uint16_t id);

bool is_new_processes();
Process* get_process_at_index(uint16_t index);

void prevent_default_exit(); // stop supervisor from automatically closing process upon ESCAPE character registering

#endif