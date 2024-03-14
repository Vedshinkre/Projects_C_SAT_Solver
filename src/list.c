#include "list.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "util.h"

/**
 * Struct for encapsulating a single list element.
 */
typedef struct ListItem {
    struct ListItem* next;  // pointer to the next element (NULL if last)
    void* data;             // pointer to the data
} ListItem;

List mkList(void) {
    List res;
    res.head = NULL;
    return res;
}

void clearList(List* s) {
   while(s->head!=0){
    ListItem * temp = s->head;
        s->head=s->head->next;
        free(temp);
    }
    if (s->head==0){
        free(s->head);
     return;
    }
    
}



void push(List* s, void* data) {
  ListItem *new_element = malloc(sizeof(ListItem));
   if (new_element == NULL){
    return;
   }
   new_element->data = data;
   new_element->next = s->head;
    s->head = new_element;

    
}


void* peek(List* s) {
    if (s->head != NULL) {
        return s->head->data;
    } else {
        return NULL;
    }
}




void pop(List* s) {
   ListItem *temp = s->head;
   
        s->head=s->head->next;
        free(temp);
   

}

char isEmpty(List* s) {
   if(s->head==0){
    return 1;
   }
   else{
    return 0;
   }

}

ListIterator mkIterator(List* list) {
    ListIterator res;
    res.list = list;
    res.prev = NULL;
    res.current = list->head;

    return res;
}

void* getCurr(ListIterator* it) {
    assert(it->current != NULL);
    return it->current->data;
}

void next(ListIterator* it) {
    assert(isValid(it));
    it->prev = it->current;
    it->current = it->current->next;
}

char isValid(ListIterator* it) { return it->current != NULL; }



