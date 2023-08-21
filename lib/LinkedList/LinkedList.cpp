#include "LinkedList.h"

LinkedList::LinkedList() {
    m_head = NULL;
    m_tail = NULL;
    m_length = 0;
    m_loop_index = 0;
    m_loop_head = NULL;
}

void LinkedList::insertItem(void* data) {
    insertItem(data, m_length);
}

void LinkedList::insertItem(void* data, uint16_t index) {
    ListItem* newItem = (ListItem*) malloc(sizeof(struct ListItem));
    newItem->data = data;

    if (index < m_length) { // placing item before another
        if (index == 0) { // replacing head
            newItem->next = m_head;
            m_head = newItem;
        }
        else { // replacing item in list
            ListItem* next_previous = getRawItem(index - 1); // head of item to push ahead
            // splice newItem into list
            newItem->next = next_previous->next;
            next_previous->next = newItem;
        }
    }
    else { // adding item to the end of the list
      // fill linked list with NULL data if inserting after end of list
        for (uint16_t i = m_length; i < index; i++) {
            ListItem* fillerItem = (ListItem*)malloc(sizeof(struct ListItem));
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

void* LinkedList::getItem(uint16_t index) {
    if (index >= m_length) return NULL; // outside list bounds
    return getRawItem(index)->data;
}

void LinkedList::removeItem(uint16_t index) {
    if (index >= m_length) return; // no removals required, so don't do anything

    ListItem* temp_head = getRawItem(index); // garunteed to be defined
    ListItem* temp_head_next = temp_head->next;

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
    else getRawItem(index - 1)->next = temp_head_next; // connect the last item before the splice to the first item after the splice

    m_length--;
}

void LinkedList::removeItemWithoutDealloc(uint16_t index) {
    if (index >= m_length) return; // no removals required, so don't do anything

    ListItem* temp_head = getRawItem(index); // garunteed to be defined
    ListItem* temp_head_next = temp_head->next;

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

void LinkedList::replaceItem(void* data, uint16_t index) {
    if (index >= m_length) { // item doesn't yet exist, so create it
        insertItem(data, index);
        return;
    }
    ListItem* temp_head = getRawItem(index);
    temp_head->data = data; // replace with new data
}

bool LinkedList::isEmpty() {
    return m_length == 0;
}

void LinkedList::loopInit() {
    m_loop_head = m_head;
    m_loop_index = 0;
}

void* LinkedList::loopNext() {
    if (!m_loop_head) {
        return NULL;
    }
    m_loop_index++;

    ListItem* temp_head = m_loop_head;
    m_loop_head = m_loop_head->next;
    return temp_head->data;
}

bool LinkedList::isLoopDone() {
    return m_loop_index == m_length;
}


ListItem* LinkedList::getRawItem(uint16_t index) {
    if (index >= m_length) return NULL; // outside list bounds
    ListItem* temp_head = m_head;
    for (uint16_t i = 0; i < index; i++) {
        temp_head = temp_head->next;
    }
    return temp_head;
}

uint16_t LinkedList::length() {
    return m_length;
}