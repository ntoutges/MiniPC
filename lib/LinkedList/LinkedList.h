#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdint.h>
#include <stdlib.h>

struct ListItem {
    ListItem* next;
    void* data; // this MUST be a pointer to something made with the 'new' keyword
};

class LinkedList {
private:
    ListItem* m_head;
    ListItem* m_tail;
    uint16_t m_length;
    uint16_t m_loop_index;
    ListItem* m_loop_head;

public:
    LinkedList();

    void insertItem(void* data); // acts as a .push operation
    void insertItem(void* data, uint16_t index);
    void removeItem(uint16_t index);
    void removeItemWithoutDealloc(uint16_t index);
    void replaceItem(void* data, uint16_t index);
    // void splice(uint16_t index, uint16_t removals); // standard splice from JS
    void* getItem(uint16_t index);

    bool isEmpty();

    void loopInit();
    void* loopNext();
    bool isLoopDone();

    uint16_t length();

private:
    ListItem* getRawItem(uint16_t index);
};

#endif