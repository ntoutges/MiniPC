#include "ScreenComponent.h"
#include "Arduino.h"

ScreenComponent::ScreenComponent(
  uint8_t x,
  uint8_t y,
  uint8_t width,
  uint8_t height
) {
  m_x = x;
  m_y = y;
  m_width = width;
  m_height = height;

  m_size = width*height;
  m_data = (char*) malloc(sizeof(char) * m_size);

  for (uint8_t i = 0; i < m_size; i++) {
    m_data[i] = 32; // fill with spaces
  }
}

ScreenComponent::~ScreenComponent() {
  free(m_data);
}

bool ScreenComponent::hasChanged() { return m_has_changed; }
void ScreenComponent::setHasChanged(bool hasChanged) { m_has_changed = hasChanged; }
uint8_t ScreenComponent::size() { return m_size; }
uint8_t ScreenComponent::width() { return m_width; }
uint8_t ScreenComponent::height() { return m_height; }
uint8_t ScreenComponent::getX() { return m_x; }
uint8_t ScreenComponent::getY() { return m_y; }

char ScreenComponent::getCharAt(uint8_t x, uint8_t y) {
  // outside bounds
  if (x < m_x || y < m_y || x >= m_x + m_width || y >= m_y + m_height) {
    return 0;
  }

  uint8_t index = (x-m_x) + m_width*(y-m_y);
  return m_data[index];
}

void ScreenComponent::setCharAt(uint8_t x, uint8_t y, char character) {
  // outside bounds
  if (x < m_x || y < m_y || x >= m_x + m_width || y >= m_y + m_height) {
    return;
  }

  uint8_t index = (x-m_x) + m_width*(y-m_y);
  m_data[index] = character;
  m_has_changed = true;
}

void ScreenComponent::setCharAt(uint8_t index, char character) {
  // outside bounds
  if (index >= m_size) {
    return;
  }

  m_data[index] = character;
  m_has_changed = true;
}

void ScreenComponent::setText(char* text) {
  uint8_t data_index = 0;
  uint8_t length = size();
  for (uint8_t i = 0; text[i] && data_index < length; i++) {
    if (text[i] == '\n') { // new line
      data_index += width() - (data_index % width()); // increment data_index to next line
    }
    else {
      setCharAt(data_index, text[i]);
      data_index++;
    }
  }

  // fill rest of characters with ' ' (32)
  for (uint8_t i = data_index; i < length; i++) {
    setCharAt(i, 32);
  }
}

void ScreenComponent::scrollText(int8_t step) {
  if (step == 0) return; // nothing happens
  m_has_changed = true;
  if (step < 0) { // scroll left
    if (step <= -m_width) { // all text scrolls off screen
      setText("");
      return;
    }
    for (uint8_t i = 0; i < m_size + step; i++) { setCharAt(i, m_data[i - step]); }
    for (uint8_t i = m_size + step; i < m_size; i++) { setCharAt(i, ' '); }
  }
  else { // scroll right
    if (step >= m_width) { // all text scrolls off screen
      setText("");
      return;
    }
    for (int8_t i = m_size - step - 1; i >= 0; i--) { setCharAt(i+step, m_data[i]); }
    for (uint8_t i = 0; i < step; i++) { setCharAt(i, ' '); }
  }
}


MenuItem::MenuItem(char* name, uint8_t value) {
  uint8_t i = 0;
  while (name[i]) { i++; } // loop until at end of name
  i++;
  // [i] now represents the length of name
  
  // copy from temp-pointer char* name into semi-permanent memory
  m_name = (char*) malloc(i);
  for (uint8_t j = 0; j < i; j++) {
    m_name[j] = name[j];
  }

  // m_name = name;
  m_value = value;
}

MenuItem::~MenuItem() {
  free(m_name);
}

char* MenuItem::getName() { return m_name; }
uint8_t MenuItem::getValue() { return m_value; }
void MenuItem::getName(char* buffer, uint8_t limit, uint8_t ellipses) {
  for (uint8_t i = 0; i <= limit; i++) {
    buffer[i] = m_name[i];
    if (!m_name[i]) { // text ends without incident
      return;
    }
  }

  buffer[limit-1] = ellipses; // replace last visible character with elipses
  buffer[limit] = 0; // null character
}

MenuComponent::MenuComponent(
  uint8_t x,
  uint8_t y,
  uint8_t width,
  uint8_t upArrow,
  uint8_t downArrow,
  uint8_t ellipses
): ScreenComponent(x,y,width,2) { // locked at a height of 2
  m_items = new LinkedList<MenuItem>();
  m_line = 0;

  m_up_arrow = upArrow;
  m_down_arrow = downArrow;
  m_ellipses = ellipses;

  setText("<EMPTY>"); // default text, as list starts empty
}

MenuComponent::~MenuComponent() {
  delete m_items;
}

void MenuComponent::addMenuItem( char* name ) {
  addMenuItem(name, m_items->length());
}

void MenuComponent::addMenuItem( char* name, uint8_t value ) {
  MenuItem* item = new MenuItem(name, value);
  m_items->insertItem(item);
  scrollMenuItem(0); // update text
}

void MenuComponent::addMenuItemBefore( char* name, uint8_t value ) {
  MenuItem* item = new MenuItem(name, value);
  m_items->insertItem(item, 0); // insert at front of list
  scrollMenuItem(0); // update text
}

void MenuComponent::removeMenuItem(uint8_t value) {
  m_items->loopInit();
  uint16_t index = 0;
  while (!m_items->isLoopDone()) {
    MenuItem* item = (MenuItem*) m_items->loopNext();
    if (item->getValue() == value) {
      m_items->removeItem(index);
      return;
    }
    index++;
  }

  if (m_items->length() == 0) setText("<EMPTY>"); // display empty message
  
  // move back up if too far down after removal
  if (m_items->length() < m_line) {
    m_line = m_items->length() - 1;
  }
  scrollMenuItem(0); // update text
}

// remove all items from list
void MenuComponent::clear() {
  // remove first item until all items removed
  while (!m_items->isEmpty()) {
    m_items->removeItem(0);
  }
  setText(""); // clear any text on screen
  m_line = 0;
}

void MenuComponent::scrollMenuItem(int8_t step) {
  if (m_items->length() == 0) { // if empty, no scrolling necessary
    setText("<EMPTY>");
    return;
  }

  // if (step == 0) return; // don't need to do anything
  if (step < 0) {
    m_line = (m_line + step < 0) ? 0 : m_line + step;
  }
  else if (step > 0) {
    uint16_t length = m_items->length();
    m_line = (m_line + step >= length) ? length-1 : m_line + step;
  }

  char* buffer = (char*) malloc(width());
  ((MenuItem*) m_items->getItem(m_line))->getName(buffer, width()-1, m_ellipses);
  
  uint8_t i = 0;
  while (buffer[i]) {
    setCharAt(i+1, 0, buffer[i]);
    i++;
  }
  while (i+1 < width()) {
    setCharAt(i+1, 0, ' '); // clear remaining characters
    i++;
  }
  
  if (m_line + 1 >= m_items->length()) buffer = "<END>";
  else ((MenuItem*) m_items->getItem(m_line+1))->getName(buffer, width()-1, m_ellipses);

  // for (uint8_t i = 0; buffer[i]; i++) {
  //   setCharAt(i+1, 1, buffer[i]);
  // }
  i = 0;
  while (buffer[i]) {
    setCharAt(i+1, 1, buffer[i]);
    i++;
  }
  while (i+1 < width()) {
    setCharAt(i+1, 1, ' '); // clear remaining characters
    i++;
  }

  if (m_line == 0) setCharAt(0,0, 32); // clear with a space character
  else setCharAt(0,0, m_up_arrow); 

  if (m_line == m_items->length()-1) setCharAt(0,1, 32); // clear with a space character
  else setCharAt(0,1, m_down_arrow);

  setHasChanged(true);
}

uint8_t MenuComponent::selectMenuItem() {
  if (m_items->isEmpty()) return -1;
  MenuItem* item = getMenuItem(m_line);
  if (item) return item->getValue();
  return -1;
}

MenuItem* MenuComponent::getMenuItem(uint16_t index) {
  MenuItem* item = (MenuItem*) m_items->getItem(index);
  if (!item) return NULL;
  return item;
}

uint8_t MenuComponent::getLine() {
  return m_line;
}


SingleMenuComponent::SingleMenuComponent(
  uint8_t x,
  uint8_t y,
  uint8_t width,
  uint8_t height,
  uint8_t items
) : ScreenComponent(x,y, width,height) {
  m_items = (char**) calloc(items, sizeof(char*));
  m_items_length = items;
  m_item_selected = 0;
}

SingleMenuComponent::~SingleMenuComponent() {
  for (uint8_t i = 0; i < m_items_length; i++) {
    if (m_items[i]) { free(m_items[i]); } // free individual char* elements
  }
  free(m_items); // free char** array
}

void SingleMenuComponent::setItem(uint8_t index, char* text) {
  if (index >= m_items_length) return; // outside working bounds
  
  if (m_items[index]) { free(m_items[index]); }
  
  uint8_t i = 0;
  while (text[i]) { i++; } // get length of text
  i++;
  // [i] now contains the size of the text (including terminating NULL)

  m_items[index] = (char*) malloc(i);
  for (uint8_t j = 0; j < i; j++) {
    m_items[index][j] = text[j]; // copy text into allocated location
  }

  if (m_item_selected == index) { selectItem(index); } // update text if selected
}

void SingleMenuComponent::selectItem(uint8_t index) {
  if (index >= m_items_length) return; // outside working bounds
  m_item_selected = index;

  setText(m_items[index]);
}


TriStateItem::TriStateItem( char symbol ) {
  m_symbol = symbol;
  m_state = 1;
}

void TriStateItem::setSymbol(char symbol) { m_symbol = symbol; }
char TriStateItem::getSymbol() { return m_symbol; }

bool TriStateItem::isNoState() { return m_state == 1; }
bool TriStateItem::isStateA() { return m_state == 0; }
bool TriStateItem::isStateB() { return m_state == 2; }

bool TriStateItem::changeStateBy(int8_t change) {
  bool wasChange = false;;
  // m_state = (m_state + change < 0) ? 0 : (m_state + change > 2) ? 2 : m_state + change;
  if (m_state + change < 0) m_state = 0;
  else if (m_state + change > 2) m_state = 2;
  else {
    m_state = m_state + change;
    wasChange = true;
  }
  return wasChange;
}

TriStateSelectorComponent::TriStateSelectorComponent(
  uint8_t x,
  uint8_t y,
  uint8_t width,
  uint8_t pointerSelected,
  uint8_t pointerSelectedA,
  uint8_t pointerSelectedB,
  uint8_t pointerUnselected,
  uint8_t pointerUnselectedA,
  uint8_t pointerUnselectedB
) : ScreenComponent(x,y,width,2) { // locked at a height of 2
  m_pointer_selected = pointerSelected;
  m_pointer_selected_a = pointerSelectedA;
  m_pointer_selected_b = pointerSelectedB;
  m_pointer_unselected = pointerUnselected;
  m_pointer_unselected_a = pointerUnselectedA;
  m_pointer_unselected_b = pointerUnselectedB;

  m_selection = 0;
  m_states = (TriStateItem**) malloc(sizeof(TriStateItem*) * width);
  for (uint8_t i = 0; i < width; i++) {
    m_states[i] = new TriStateItem(' '); // default symbol is a space
  }

  updateText();
}

TriStateSelectorComponent::~TriStateSelectorComponent() {
  for (uint8_t i = 0; i < width(); i++) {
    delete m_states[i];
  }
  free(m_states);
}

void TriStateSelectorComponent::setStateSymbol(uint8_t index, char symbol) {
  if (index >= width()) return; // invalid index
  m_states[index]->setSymbol(symbol);

  setCharAt(index, 0, symbol); // update symbol
  updateText();
}

void TriStateSelectorComponent::scrollPointer(int8_t step) {
  uint8_t w = width()-1;
  uint8_t old_selection = m_selection;
  m_selection = (m_selection + step < 0) ? 0 : (m_selection + step > w) ? w : m_selection + step;

  if (m_selection == old_selection) return; // unchanged
  updateText();
}

bool TriStateSelectorComponent::changeItemState(int8_t change) {
  bool wasChange = m_states[m_selection]->changeStateBy(change);
  updateText();
  return wasChange;
}

bool TriStateSelectorComponent::isStateA() {
  return m_states[m_selection]->isStateA();
}

bool TriStateSelectorComponent::isStateB() {
  return m_states[m_selection]->isStateB();
}

uint8_t TriStateSelectorComponent::getCursor() {
  return m_selection;
}

void TriStateSelectorComponent::updateText() {
  uint8_t w = width();
  char symbol;
  uint8_t pointerType;
  for (uint8_t i = 0; i < w; i++) {
    symbol = m_states[i]->getSymbol();
    
    if (m_selection == i) { // this is selected
      if (m_states[i]->isStateA()) pointerType = m_pointer_selected_a;
      else if (m_states[i]->isStateB()) pointerType = m_pointer_selected_b;
      else pointerType = m_pointer_selected;
    }
    else { // this is not selected
      if (m_states[i]->isStateA()) pointerType = m_pointer_unselected_a;
      else if (m_states[i]->isStateB()) pointerType = m_pointer_unselected_b;
      else pointerType = m_pointer_unselected;
    }

    setCharAt(i,0, symbol);
    setCharAt(i,1, pointerType);
  }
  setHasChanged(true);
}