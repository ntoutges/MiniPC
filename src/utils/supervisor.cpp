#include "supervisor.h"

LinkedList *supervisor_list;
LinkedList *supervisor_exit_queue;
Process* supervisor_curr_active_process;
Process* supervisor_next_active_process;
unsigned long supervisor_timer;
uint16_t supervisor_ms_per_tick;
bool supervisor_new_processes;

void supervisor_init(uint16_t ms_per_tick) {
  supervisor_ms_per_tick = ms_per_tick;
  supervisor_list = new LinkedList();
  supervisor_exit_queue = new LinkedList();

  supervisor_curr_active_process = NULL;
  supervisor_next_active_process = NULL;
  supervisor_new_processes = true; // technically?
}

void supervisor_tick(unsigned long millis) {
  if (millis < supervisor_timer) return; // wait for timeout

  supervisor_timer += supervisor_ms_per_tick;

  supervisor_list->loopInit();
  Process *process;

  // tick processes
  while (!supervisor_list->isLoopDone()) {
    process = (Process*)supervisor_list->loopNext();
    if (process->isTickable()) {
      process->tick(millis);
    }
  }

  // switching from one active process to the next
  if (supervisor_curr_active_process != supervisor_next_active_process) {
    if (supervisor_curr_active_process && supervisor_curr_active_process->hasGUI()) { // implies render function
      supervisor_curr_active_process->render(false); // stop rendering old
    }
    if (supervisor_next_active_process && supervisor_next_active_process->hasGUI()) { // implies render function
      supervisor_next_active_process->render(true); // start rendering new
    }
    supervisor_curr_active_process = supervisor_next_active_process; // new becomes current/old
  }

  // exit exitable processes
  while (supervisor_exit_queue->length()) {
    process = (Process*)supervisor_exit_queue->getItem(0);
    if (process->exit(millis)) {                          // when this returns true, the process has exited
      // by this point, current and next processes will be the same, so both can be set back to NULL
      if (process == supervisor_curr_active_process) {
        supervisor_curr_active_process = NULL;
        supervisor_next_active_process = NULL;
      }
      supervisor_exit_queue->removeItemWithoutDealloc(0); // remove from exit queue
      _removeProcess(process->getId());
    }
  }
}

void supervisor_input(uint8_t value) {
  if (supervisor_curr_active_process->hasGUI()) { // implies input function
    supervisor_curr_active_process->input(value);
  }
}

void generateProcess(
  uint16_t delay,
  void (*init)(Process*),
  void (*tick)(unsigned long millis),
  bool (*exit)(unsigned long millis)
) {
  generateProcess(
    delay,
    init,
    tick,
    exit,
    NULL,
    NULL,
    NULL
  );
}

void generateProcess(
  uint16_t delay,
  void (*init)(Process*),
  void (*tick)(unsigned long millis),
  bool (*exit)(unsigned long millis),
  Process *processBuffer
) {
  generateProcess(
    delay,
    init,
    tick,
    exit,
    NULL,
    NULL,
    processBuffer
  );
}

void generateProcess(
  uint16_t delay,
  void (*init)(Process*),
  void (*tick)(unsigned long millis),
  bool (*exit)(unsigned long millis),
  void (*input)(uint8_t value),
  void (*render)(bool is_rendering),
  Process *processBuffer
) {
  uint16_t process_id = generate_process_id();
  uint16_t process_index = get_process_index(process_id);

  Process *process = new Process(
    process_id,
    delay,
    init,
    tick,
    exit,
    input,
    render
  );

  supervisor_list->insertItem(
    process,
    process_index
  );

  if (processBuffer) processBuffer = process;
  supervisor_next_active_process = process;

  supervisor_new_processes = true;
}

void closeProcess(uint16_t id) {
  supervisor_list->loopInit();
  Process *process;
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
  Process *process;
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
    temp_process = (Process*) supervisor_list->loopNext();
    if (temp_process->getId() == id) { // found process of the correct id
      supervisor_next_active_process = temp_process;
      return;
    }
  }
}

uint16_t generate_process_id() {
  supervisor_list->loopInit();
  Process *process;
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
  Process *process;
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
  return (Process*) supervisor_list->getItem(index);
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
  if (!m_is_initialized)
  {
    m_init(this);
    m_is_initialized = true;
  }
  else
  {
    m_tick(millis);
  }
}

void Process::render(bool is_rendering) { m_render(is_rendering); }
bool Process::exit(unsigned long millis) { return m_exit(millis); }
void Process::input(uint8_t value) { return m_input(value); }
