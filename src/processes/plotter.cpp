#include "plotter.h"
#include <Arduino.h>

uint8_t plotter_graphs;
TriStateSelectorComponent* plotter_selector;
ScreenComponent* plotter_read_value_from; // stores what pin reading from
ScreenComponent* plotter_read_value_at; // stores pin value
uint8_t* plotter_outputs;
float plotter_current_value;
bool plotter_is_running = false;
bool plotter_is_active = false;

void plotter_init(Process* process) {
  // none of these should be initialized yet
  plotter_selector = new TriStateSelectorComponent(0,0, 4, 0,1,2, 3,4,5);
  plotter_read_value_from = new ScreenComponent(5,0, 2,1);
  plotter_read_value_at = new ScreenComponent(5,1, 13,1);
  plotter_read_value_from->setCharAt(0,'A');

  if (plotter_is_running) { return; } // don't reset anything, as plotter is still running
  
  plotter_outputs = (uint8_t*) malloc(4 * sizeof(uint8_t));
  for (uint8_t i = 0; i < 4; i++) {
      plotter_outputs[i] = 255; // reset all
  }

  plotter_is_running = true;
  plotter_clear();
}

void plotter_tick(unsigned long millis) {
  char buffer[14];
  char buffer2[4];
  uint8_t currentCursor = -1;
  if (plotter_is_active) currentCursor = plotter_selector->getCursor();
  for (uint8_t i = 0; i < 4; i++) {
    if (plotter_outputs[i] != 255) {
      int value = analogRead(i);
      plotter_send(plotter_outputs[i], value);

      if (i == currentCursor) {
        float voltage = 5.0 * value / 1023.0; // this is apparently the official conversion... 
        dtostrf(voltage, 4,2, buffer2);
        snprintf(buffer, 13, "%4d: %sV", value, buffer2);
        plotter_read_value_at->setText(buffer);
      }
    }
  }
}

bool plotter_exit(unsigned long millis) {
  bool isSending = false;
  for (uint8_t i = 0; i < 4; i++) {
    if (plotter_outputs[i] != 255) {
      isSending = true;
    }
  }

  if (!isSending) { // only actually close if not currently running a graph
    plotter_is_running = false;
    free(plotter_outputs);
    plotter_clear();
    return true;
  }
  return false; // only allow program to close if no plots running
}

void plotter_input(uint8_t value) {
    screen_keep_alive();
    
    bool wasChange = false;
    bool wasA = plotter_selector->isStateA();
    bool wasB = plotter_selector->isStateB();

    switch (value) {
        case INPUTS_ARROW_UP:
            wasChange = plotter_selector->changeItemState(-1);
            break;
        case INPUTS_ARROW_DOWN:
            wasChange = plotter_selector->changeItemState(1);
            plotter_read_value_at->setText("");
            break;
        case INPUTS_ARROW_LEFT:
            plotter_selector->scrollPointer(-1);
            plotter_read_value_from->setCharAt(1,'0'+plotter_selector->getCursor());
            plotter_read_value_at->setText("");
            break;
        case INPUTS_ARROW_RIGHT:
            plotter_selector->scrollPointer(1);
            plotter_read_value_from->setCharAt(1,'0'+plotter_selector->getCursor());
            plotter_read_value_at->setText("");
            break;
    }

    // B state might do something in the future, for now, it acts the same as no state
    if (wasChange) {
        uint8_t index = plotter_selector->getCursor();
        if (wasA) { // A -> _
            plotter_outputs[index] = 255; // reset
        }
        else if (wasB) { // B -> _
            plotter_outputs[index] = 255; // reset
        }
        else if (plotter_selector->isStateA()) { // _ -> A
            plotter_outputs[index] = 0; // set
        }
        else { // if (plotter_selector->isStateB()) { // _ -> B
            plotter_outputs[index] = 255; // reset
        }
        plotter_update_selector();
    }
}

void plotter_render(bool is_rendering) {
  plotter_is_active = is_rendering;
  if (is_rendering) {
    screen_add_component(plotter_selector);
    screen_add_component(plotter_read_value_from);
    screen_add_component(plotter_read_value_at);
    screen_create_char(0, chr_selected);
    screen_create_char(1, chr_selected_a);
    screen_create_char(2, chr_selected_b);
    screen_create_char(3, chr_unselected);
    screen_create_char(4, chr_unselected_a);
    screen_create_char(5, chr_unselected_b);
  }
}

void plotter_update_selector() {
  uint8_t plot_id = 0;
  for (uint8_t i = 0; i < 4; i++) {
    if (plotter_outputs[i] != 255) {
      plotter_outputs[i] = plot_id; // update id so all are unique and in order
      plotter_selector->setStateSymbol(i, plot_id);
      plotter_selector->setStateSymbol(i, '0' + plot_id);
      plot_id++;
    }
    else {
      plotter_selector->setStateSymbol(i, ' ');
    }
  }
  plotter_set_graph_count(plot_id); // this number now holds the amount of plots
}

void plotter_clear() {
  Serial1.write('c');
  plotter_graphs = 0;
}

void plotter_add() {
  Serial1.write('a');
  plotter_graphs++;
}

void plotter_pop() {
  if (plotter_graphs == 1) { // firmware on plotter doesn't allow last plot to be cleared through popping
    plotter_clear();
    return;
  }
  Serial1.write('q');
  plotter_graphs--;
}

void plotter_send(uint8_t plot, float data) {
    if (plot >= 4) return; // invalid plot

    while (plot >= plotter_graphs) { plotter_add(); }

    Serial1.write('0' + plot);
    Serial1.write((char*) &data, 4); // treat float as a buffer to be sent
}

void plotter_set_graph_count(uint8_t plots) {
    while (plots > plotter_graphs) { plotter_add(); } // not enough graphs
    while (plotter_graphs > plots) { plotter_pop(); } // too many graphs
}