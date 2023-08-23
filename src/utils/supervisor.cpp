#include "supervisor.h"
#include "screen.h" // this is in .cpp instead of .h to avoid circular dependencies problem (somehow...)

LinkedList<Process>* supervisor_list;
LinkedList<Process>* supervisor_exit_queue;
Process* supervisor_curr_active_process;
Process* supervisor_next_active_process;
unsigned long supervisor_timer;
uint16_t supervisor_ms_per_tick;
bool supervisor_new_processes;
bool supervisor_is_exit_prevented;

void supervisor_init(uint16_t ms_per_tick) {
    supervisor_ms_per_tick = ms_per_tick;
    supervisor_list = new LinkedList<Process>();
    supervisor_exit_queue = new LinkedList<Process>();

    supervisor_curr_active_process = NULL;
    supervisor_next_active_process = NULL;
    supervisor_new_processes = true; // technically?
}

void supervisor_tick(unsigned long millis) {
    if (millis < supervisor_timer) return; // wait for timeout

    supervisor_timer += supervisor_ms_per_tick;

    supervisor_list->loopInit(1);
    Process* process;

    // tick processes
    while (!supervisor_list->isLoopDone(1)) {
        process = (Process*)supervisor_list->loopNext(1);
        if (process->isTickable()) {
            process->tick(millis); // crash is occuring here
        }
    }

    // switching from one active process to the next
    if (supervisor_curr_active_process != supervisor_next_active_process) {
        if (supervisor_curr_active_process) { // implies render function
            supervisor_curr_active_process->render(false); // stop rendering old
            screen_clear_without_dealloc(); // clear screen and remove components from previous process
        }

        if (supervisor_next_active_process) {
            supervisor_next_active_process->init(supervisor_next_active_process); // init new process
            if (supervisor_next_active_process) { // implies render function
                supervisor_next_active_process->render(true); // start rendering new
            }
        }
        supervisor_curr_active_process = supervisor_next_active_process; // new becomes current/old
    }

    // exit exitable processes
    while (supervisor_exit_queue->length()) {
        process = (Process*)supervisor_exit_queue->getItem(0);
        if (process->exit(millis)) {                          // when this returns true, the process has exited
            supervisor_exit_queue->removeItemWithoutDealloc(0); // remove from exit queue
            _removeProcess(process->getId());

            // by this point, current and next processes will be the same, so both can be set back to NULL
            if (process == supervisor_curr_active_process) {
                // supervisor_curr_active_process = NULL; // don't even try to render that which no longer exists
                supervisor_next_active_process = _getLastGUIProcess(); // jump back to last program with GUI opened
                // supervisor_next_active_process = NULL;
            }
            supervisor_new_processes = true;
        }
    }
}

Process* _getLastGUIProcess() {
    supervisor_list->loopInit();
    Process* lastWithGUI = NULL;
    Process* temp;
    while (!supervisor_list->isLoopDone()) {
        temp = (Process*)supervisor_list->loopNext();
        if (temp->hasGUI()) {
            lastWithGUI = temp;
        }
    }
    return lastWithGUI;
}

void supervisor_input(uint8_t value) {
    if (supervisor_curr_active_process->hasGUI()) { // implies input function
        supervisor_is_exit_prevented = false;
        supervisor_curr_active_process->input(value);
        
        if (value == INPUTS_ESCAPE && !supervisor_is_exit_prevented) {
            closeProcess(supervisor_curr_active_process->getId());
        }
    }
}

ProcessInfo* generateProcess(
    uint16_t delay,
    void (*init)(Process*),
    void (*tick)(unsigned long millis),
    bool (*exit)(unsigned long millis)
) {
    return generateProcess(
        delay,
        init,
        tick,
        exit,
        NULL,
        NULL
    );
}

ProcessInfo* generateProcess(
    uint16_t delay,
    void (*init)(Process*),
    void (*tick)(unsigned long millis),
    bool (*exit)(unsigned long millis),
    void (*input)(uint8_t value),
    void (*render)(bool is_rendering)
) {
    ProcessInfo* processInfo = (ProcessInfo*) malloc(sizeof(struct ProcessInfo));
    processInfo->delay = delay;
    processInfo->init = init;
    processInfo->tick = tick;
    processInfo->exit = exit;
    processInfo->input = input;
    processInfo->render = render;

    return processInfo;
}

Process* runProcess(ProcessInfo* processInfo) {
    Process* process = runProcessWithoutDealloc(processInfo);
    free(processInfo);
    return process;
}

Process* runProcessWithoutDealloc(ProcessInfo* processInfo) {
    uint16_t process_id = generate_process_id();
    uint16_t process_index = get_process_index(process_id);

    Process* process = new Process(
        process_id,
        processInfo->delay,
        processInfo->init,
        processInfo->tick,
        processInfo->exit,
        processInfo->input,
        processInfo->render
    );

    supervisor_list->insertItem(
        process
        // process_index
    );

    if (process->hasGUI()) supervisor_next_active_process = process;
    supervisor_new_processes = true;

    return process;
}

void closeProcess(uint16_t id) {
    supervisor_list->loopInit();
    Process* process;
    uint16_t index = 0;

    while (!supervisor_list->isLoopDone())
    {
        process = (Process*)supervisor_list->loopNext();
        if (process->getId() == id)
        {
            supervisor_exit_queue->insertItem(process); // just remember to eventually remove these
            return;
        }
        index++;
    }
}

void _removeProcess(uint16_t id) {
    supervisor_list->loopInit();
    Process* process;
    uint16_t index = 0;

    while (!supervisor_list->isLoopDone())
    {
        process = (Process*)supervisor_list->loopNext();
        if (process->getId() == id)
        {
            supervisor_list->removeItem(index);
            return;
        }
        index++;
    }

    supervisor_new_processes = true;
}

void setActiveProcess(uint16_t id) {
    supervisor_list->loopInit();
    Process* temp_process;
    while (!supervisor_list->isLoopDone()) {
        temp_process = (Process*)supervisor_list->loopNext();
        if (temp_process->getId() == id) { // found process of the correct id
            supervisor_next_active_process = temp_process;
            return;
        }
    }
}

uint16_t generate_process_id() {
    supervisor_list->loopInit();
    Process* process;
    uint16_t next_available_id = 0;
    while (!supervisor_list->isLoopDone())
    {
        process = (Process*)supervisor_list->loopNext();
        if (next_available_id != process->getId())
        { // 'next_available_id' is available
            return next_available_id;
        }
        next_available_id = process->getId() + 1;
    }
    return next_available_id;
}

uint16_t get_process_index(uint16_t id) {
    supervisor_list->loopInit();
    Process* process;
    uint16_t index = 0;
    while (!supervisor_list->isLoopDone())
    {
        process = (Process*)supervisor_list->loopNext();
        if (id < process->getId())
        {
            return index;
        }
        index++;
    }
    return index;
}

bool is_new_processes() {
    bool temp = supervisor_new_processes;
    supervisor_new_processes = false;
    return temp;
}

Process* get_process_at_index(uint16_t index) {
    return (Process*)supervisor_list->getItem(index);
}

void prevent_default_exit() {
    supervisor_is_exit_prevented = true;
}

Process::Process(
    uint16_t process_id,
    uint16_t delay,
    void (*init)(Process*),
    void (*tick)(unsigned long millis),
    bool (*exit)(unsigned long millis)
) {
    this->init(
        process_id,
        delay,
        init,
        tick,
        exit,
        NULL,
        NULL
    );
}

Process::Process(
    uint16_t process_id,
    uint16_t delay,
    void (*init)(Process*),
    void (*tick)(unsigned long millis),
    bool (*exit)(unsigned long millis),
    void (*input)(uint8_t value),
    void (*render)(bool is_rendering)
) {
    this->init(
        process_id,
        delay,
        init,
        tick,
        exit,
        input,
        render
    );
}

uint16_t Process::getId() { return m_process_id; }

void Process::init(
    uint16_t process_id,
    uint16_t delay,
    void (*init)(Process*),
    void (*tick)(unsigned long millis),
    bool (*exit)(unsigned long millis),
    void (*input)(uint8_t value),
    void (*render)(bool is_rendering)
) {
    m_process_id = process_id;
    m_delay = delay;
    m_init = init;
    m_tick = tick;
    m_exit = exit;
    m_input = input;
    m_render = render;

    m_delay_tracker = 1;
    m_is_initialized = false;
}

bool Process::isTickable() {
    m_delay_tracker++;
    if (!m_delay || m_delay_tracker >= m_delay)
    {
        m_delay_tracker = 0;
        return true;
    }
    return false;
}
bool Process::hasGUI() { return m_render; }

void Process::tick(unsigned long millis) {
    if (!m_is_initialized) {
        init(this);
    }
    else {
        m_tick(millis);
    }
}

void Process::init(Process* process) {
    if (m_is_initialized) return; // don't try to initialize again
    m_init(process);
    m_is_initialized = true;
}
void Process::render(bool is_rendering) { m_render(is_rendering); }
bool Process::exit(unsigned long millis) { return m_exit(millis); }
void Process::input(uint8_t value) { return m_input(value); }
