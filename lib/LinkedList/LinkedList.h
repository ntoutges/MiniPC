#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdint.h>
#include <stdlib.h>

#define LINKED_LIST_LOOP_INDICIES 3

template <class T>
struct ListItem {
    ListItem* next;
    T* data;
};

template <class T>
class LinkedList {
private:
    ListItem<T>* m_head;
    ListItem<T>* m_tail;
    uint16_t m_length;
    uint16_t m_loop_index[LINKED_LIST_LOOP_INDICIES];
    ListItem<T>* m_loop_head[LINKED_LIST_LOOP_INDICIES];

public:
    LinkedList();

    void insertItem(T* data); // acts as a .push operation
    void insertItem(T* data, uint16_t index);
    void removeItem(uint16_t index);
    void removeItemWithoutDealloc(uint16_t index);
    void replaceItem(T* data, uint16_t index);
    // void splice(uint16_t index, uint16_t removals); // standard splice from JS
    T* getItem(uint16_t index);

    bool isEmpty();
    bool isInList(T* data);

    // default index of 0
    void loopInit();
    T* loopNext();
    bool isLoopDone();

    void loopInit(uint8_t index);
    T* loopNext(uint8_t index);
    bool isLoopDone(uint8_t index);

    uint16_t length();

private:
    ListItem<T>* getRawItem(uint16_t index);
};

template <class T>
LinkedList<T>::LinkedList() {
    m_head = NULL;
    m_tail = NULL;
    m_length = 0;
    // m_loop_index = ;
    for (uint8_t i = 0; i < LINKED_LIST_LOOP_INDICIES; i++) {
        m_loop_index[i] = 0;
        m_loop_head[i] = NULL;
    }
}

template <class T>
void LinkedList<T>::insertItem(T* data) {
    insertItem(data, m_length);
}

template <class T>
void LinkedList<T>::insertItem(T* data, uint16_t index) {
    ListItem<T>* newItem = (ListItem<T>*) malloc(sizeof(struct ListItem<T>));
    newItem->data = data;

    if (index < m_length) { // placing item before another
        if (index == 0) { // replacing head
            newItem->next = m_head;
            m_head = newItem;
        }
        else { // replacing item in list
            ListItem<T>* next_previous = getRawItem(index - 1); // head of item to push ahead
            // splice newItem into list
            newItem->next = next_previous->next;
            next_previous->next = newItem;
        }
    }
    else { // adding item to the end of the list
        // fill linked list with NULL data if inserting after end of list
        for (uint16_t i = m_length; i < index; i++) {
            ListItem<T>* fillerItem = (ListItem<T>*) malloc(sizeof(struct ListItem<T>));
            fillerItem->data = NULL;
            // ->next doesn't matter, because it will be garunteed taken up by newItem

            // shift list to include fillerItem
            m_tail->next = fillerItem;
            m_tail = fillerItem;
            m_length++;
        }

        newItem->next = NULL; // garunteed at end, therefore next item will be nothing
        if (isEmpty()) { // replacing head and tail
            m_head = newItem;
            m_tail = m_head;
        }
        else { // pushing to end of list
            m_tail->next = newItem;
            m_tail = newItem;
        }
    }
    m_length++;
}

// void LinkedList::splice(uint16_t index, uint16_t removals) {
//   if (removals == 0 || index >= m_length) return; // no removals required, so don't do anything

//   ListItem* temp_head = getRawItem(index); // garunteed to be defined
//   ListItem* temp_head_next;

//   for (uint16_t i = 0; i < removals && temp_head; i++) {
//     temp_head_next = temp_head->next;
//     free(temp_head);
//     temp_head = temp_head_next;
//     m_length--;
//   }

//   // by this point, temp_head is either null, or the next item
//   if (index == 0) m_head = temp_head; // replacing head
//   else getRawItem(index-1)->next = temp_head; // connect the last item before the splice to the first item after the splice 
// }

template <class T>
T* LinkedList<T>::getItem(uint16_t index) {
    if (index >= m_length) return NULL; // outside list bounds
    return getRawItem(index)->data;
}

template <class T>
void LinkedList<T>::removeItem(uint16_t index) {
    if (index >= m_length) return; // no removals required, so don't do anything

    ListItem<T>* temp_head = getRawItem(index); // garunteed to be defined
    ListItem<T>* temp_head_next = temp_head->next;

    // get rid of any memory used by the ListItem
    delete temp_head->data;
    free(temp_head);

    // temp_head_next is either null, or the next item
    if (index == 0) {
        m_head = temp_head_next; // replacing head
        if (m_head == NULL) {
            m_tail = NULL;
        }
    }
    else {
        ListItem<T>* lastPreviousItem = getRawItem(index - 1);
        lastPreviousItem->next = temp_head_next; // connect the last item before the splice to the first item after the splice
        if (index + 1 == m_length) { // removing last item
            m_tail = lastPreviousItem;
        }
    }

    m_length--;
}

template <class T>
void LinkedList<T>::removeItemWithoutDealloc(uint16_t index) {
    if (index >= m_length) return; // no removals required, so don't do anything

    ListItem<T>* temp_head = getRawItem(index); // garunteed to be defined
    ListItem<T>* temp_head_next = temp_head->next;

    free(temp_head);

    // temp_head_next is either null, or the next item
    if (index == 0) {
        m_head = temp_head_next; // replacing head
        if (m_head == NULL) {
            m_tail = NULL;
        }
    }
    else getRawItem(index - 1)->next = temp_head_next; // connect the last item before the splice to the first item after the splice

    m_length--;
}

template <class T>
void LinkedList<T>::replaceItem(T* data, uint16_t index) {
    if (index >= m_length) { // item doesn't yet exist, so create it
        insertItem(data, index);
        return;
    }
    ListItem<T>* temp_head = getRawItem(index);
    temp_head->data = data; // replace with new data
}

template <class T>
bool LinkedList<T>::isEmpty() {
    return m_length == 0;
}

template <class T>
bool LinkedList<T>::isInList(T* data) {
    ListItem<T>* temp_head = m_head;
    while (temp_head) {
        if (temp_head->data == data) return true;
        temp_head = temp_head->next;
    }
    return false;
}

template <class T>
void LinkedList<T>::loopInit() { return loopInit(0); }

template <class T>
void LinkedList<T>::loopInit(uint8_t index) {
    m_loop_head[index] = m_head;
    m_loop_index[index] = 0;
}

template <class T>
T* LinkedList<T>::loopNext() { return loopNext(0); }

template <class T>
T* LinkedList<T>::loopNext(uint8_t index) {
    if (!m_loop_head) {
        return NULL;
    }
    m_loop_index[index]++;
    return getItem(m_loop_index[index] - 1);

    ListItem<T>* temp_head = m_loop_head[index];
    m_loop_head[index] = m_loop_head[index]->next;
    return temp_head->data;
}

template <class T>
bool LinkedList<T>::isLoopDone() { return isLoopDone(0); }

template <class T>
bool LinkedList<T>::isLoopDone(uint8_t index) {
    return m_loop_index[index] == m_length;
}

template <class T>
ListItem<T>* LinkedList<T>::getRawItem(uint16_t index) {
    ListItem<T>* temp_head = m_head;
    for (uint16_t i = 0; i < index; i++) {
        temp_head = temp_head->next;
    }
    return temp_head;
}

template <class T>
uint16_t LinkedList<T>::length() {
    return m_length;
}

#endif