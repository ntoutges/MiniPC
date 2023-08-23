#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <LinkedList.h>

class ScreenComponent {
  uint8_t m_x;
  uint8_t m_y;
  uint8_t m_width;
  uint8_t m_height;
  
  char* m_data;
  uint8_t m_size;

  bool m_has_changed;

  public:
  ScreenComponent(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height
  );
  ~ScreenComponent();

  bool hasChanged();
  void setHasChanged(bool hasChanged);
  char getCharAt(uint8_t x, uint8_t y);
  void setCharAt(uint8_t x, uint8_t y, char character);
  void setCharAt(uint8_t index, char character);
  void setText(char* text);
  
  uint8_t size();
  uint8_t width();
  uint8_t height();
  uint8_t getX();
  uint8_t getY();
};

class MenuItem {
  char* m_name;
  uint8_t m_value;

  public:
  MenuItem(char* name, uint8_t value);
  ~MenuItem();
  char* getName();
  void getName(char* buffer, uint8_t limit, uint8_t ellipses); // limits name length to limit (buffer must be 1+ this value) (adds ellipses char)
  uint8_t getValue();
};

class MenuComponent : public ScreenComponent {
  LinkedList<MenuItem>* m_items;
  uint8_t m_line;
  uint8_t m_up_arrow;
  uint8_t m_down_arrow;
  uint8_t m_ellipses;

  public:
  MenuComponent(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t upArrow,
    uint8_t downArrow,
    uint8_t ellipses
  );
  ~MenuComponent();
  void addMenuItem( char* name ); // use current array length as value (only works if not removing items from array, others duplicate values bound to arrise)
  void addMenuItem( char* name, uint8_t value );
  void addMenuItemBefore( char* name, uint8_t value );
  void removeMenuItem(uint8_t value);
  void clear();
  void scrollMenuItem(int8_t step);
  uint8_t getLine();
  uint8_t selectMenuItem(); // return value of item

  MenuItem* getMenuItem(uint16_t index);

};

class SingleMenuComponent : public ScreenComponent {
  char** m_items;
  uint8_t m_items_length;
  uint8_t m_item_selected;

  public:
  SingleMenuComponent(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    uint8_t items
  );
  ~SingleMenuComponent();
  void setItem(uint8_t index, char* text);
  void selectItem(uint8_t index);
};

class TriStateItem {
  char m_symbol;
  uint8_t m_state;

  public:
  TriStateItem( char symbol );

  void setSymbol(char symbol);
  char getSymbol();

  bool isNoState();
  bool isStateA();
  bool isStateB();

  bool changeStateBy(int8_t change);
};

class TriStateSelectorComponent : public ScreenComponent {
  // LinkedList* m_items;
  TriStateItem** m_states;
  uint8_t m_selection;

  // sequence: selected1 -> unselected -> selected2
  uint8_t m_pointer_selected;
  uint8_t m_pointer_selected_a;
  uint8_t m_pointer_selected_b;
  
  uint8_t m_pointer_unselected;
  uint8_t m_pointer_unselected_a;
  uint8_t m_pointer_unselected_b;

  public:
  TriStateSelectorComponent(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t pointerSelected,
    uint8_t pointerSelectedA,
    uint8_t pointerSelectedB,
    uint8_t pointerUnselected,
    uint8_t pointerUnselectedA,
    uint8_t pointerUnselectedB
  );
  ~TriStateSelectorComponent();
  
  void setStateSymbol(uint8_t index, char symbol);
  void scrollPointer(int8_t step);
  bool changeItemState(int8_t change);
  
  bool isStateA();
  bool isStateB();

  uint8_t getCursor();

  private:
  void updateText();
};