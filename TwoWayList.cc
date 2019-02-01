#ifndef _TWO_WAY_LIST_C
#define _TWO_WAY_LIST_C

#include "TwoWayList.h"

#include <iostream>
#include <stdlib.h>

using namespace std;


// create an alias of the given TwoWayList
template<class Type>
TwoWayList<Type>::TwoWayList(TwoWayList &me) {

    list = new(std::nothrow) Header;
    if (list == NULL) {
        cout << "ERROR : Not enough memory. EXIT !!!\n";
        exit(1);
    }

    list->first = me.list->first;
    list->last = me.list->last;
    list->current = me.list->current;
    list->leftSize = me.list->leftSize;
    list->rightSize = me.list->rightSize;
}

// basic constructor function
template<class Type>
TwoWayList<Type>::TwoWayList() {

    // allocate space for the header
    list = new(std::nothrow) Header;
    if (list == NULL) {
        cout << "ERROR : Not enough memory. EXIT !!!\n";
        exit(1);
    }

    // set up the initial values for an empty list
    list->first = new(std::nothrow) Node;
    if (list->first == NULL) {
        cout << "ERROR : Not enough memory. EXIT !!!\n";
        exit(1);
    }

    list->last = new(std::nothrow) Node;
    if (list->last == NULL) {
        cout << "ERROR : Not enough memory. EXIT !!!\n";
        exit(1);
    }

    list->current = list->first;
    list->leftSize = 0;
    list->rightSize = 0;
    list->first->next = list->last;
    list->last->previous = list->first;

//    cout << "Current pointer: " << list->current << endl;
//    cout << "Current Next pointer: " << list->current->next << endl;
//    cout << "First pointer: " << list->first << endl;
//    cout << "First Next pointer: " << list->first->next << endl;
//    cout << "Last pointer: " << list->last << endl;
//    cout << "Last Prev pointer: " << list->last->previous << endl;


}

// basic deconstructor function
template<class Type>
TwoWayList<Type>::~TwoWayList() {

    // remove everything
    MoveToStart();
    while (RightLength() > 0) {
        Type temp;
        Remove(&temp);
    }

    // kill all the nodes
    for (int i = 0; i <= list->leftSize + list->rightSize; i++) {
        list->first = list->first->next;
        delete list->first->previous;
    }
    delete list->first;

    // kill the header
    delete list;

}

// swap operator
template<class Type>
void
TwoWayList<Type>::operator&=(TwoWayList &List) {

    Header *temp = List.list;
    List.list = list;
    list = temp;

}

// make the first node the current node
template<class Type>
void
TwoWayList<Type>::MoveToStart() {

    // check for empty list
    if(list->first->next == list->last) {
        cout << "Empty list in move to start" << endl;
        list->current = list->first;
    } else {
        list->current = list->first->next;
    }

    list->rightSize += list->leftSize;
    list->leftSize = 0;



}

// make the first node the current node
template<class Type>
void
TwoWayList<Type>::MoveToFinish() {

    // check for empty list
    if(list->last->previous == list->first) {
        list->current = list->last;
    } else {
        list->current = list->last->previous;
    }

    list->leftSize += list->rightSize;
    list->rightSize = 0;

}

// determine the number of items to the left of the current node
template<class Type>
int
TwoWayList<Type>::LeftLength() {
    return (list->leftSize);
}

// determine the number of items to the right of the current node
template<class Type>
int
TwoWayList<Type>::RightLength() {
    return (list->rightSize);
}

// swap the right sides of two lists
template<class Type>
void
TwoWayList<Type>::SwapRights(TwoWayList &List) {

    // swap out everything after the current nodes
    Node *left_1 = list->current;
    Node *right_1 = list->current->next;
    Node *left_2 = List.list->current;
    Node *right_2 = List.list->current->next;

    left_1->next = right_2;
    right_2->previous = left_1;
    left_2->next = right_1;
    right_1->previous = left_2;

    // set the new endpoints
    Node *temp = list->last;
    list->last = List.list->last;
    List.list->last = temp;

    int tempint = List.list->rightSize;
    List.list->rightSize = list->rightSize;
    list->rightSize = tempint;

}

// swap the leftt sides of the two lists
template<class Type>
void
TwoWayList<Type>::SwapLefts(TwoWayList &List) {

    // swap out everything after the current nodes
    Node *left_1 = list->current;
    Node *right_1 = list->current->next;
    Node *left_2 = List.list->current;
    Node *right_2 = List.list->current->next;

    left_1->next = right_2;
    right_2->previous = left_1;
    left_2->next = right_1;
    right_1->previous = left_2;

    // set the new frontpoints
    Node *temp = list->first;
    list->first = List.list->first;
    List.list->first = temp;

    // set the new current nodes
    temp = list->current;
    list->current = List.list->current;
    List.list->current = temp;

    int tempint = List.list->leftSize;
    List.list->leftSize = list->leftSize;
    list->leftSize = tempint;
}

// move forwards through the list 
template<class Type>
void
TwoWayList<Type>::Advance() {

    // check if we reached the end of the list
    if (list->current->next == list->last) {
        MoveToStart();
    } else {
        (list->rightSize)--;
        (list->leftSize)++;
        list->current = list->current->next;
    }

}

// move backwards through the list
template<class Type>
void
TwoWayList<Type>::Retreat() {

    // check if we reached the beginning of the list
    if(list->current->previous == list->first) {
        MoveToFinish();
    } else {
        (list->rightSize)++;
        (list->leftSize)--;
        list->current = list->current->previous;
    }

}

// insert an item at the current poition
template<class Type>
void
TwoWayList<Type>::Insert(Type *Item) {

//    cout << "INSERTING" << endl;

    Node *temp = new(std::nothrow) Node;
    if (temp == NULL) {
        cout << "ERROR : Not enough memory. EXIT !!!\n";
        exit(1);
    }

//    cout << "Current pointer: " << list->current << endl;
//    cout << "First pointer: " << list->first << endl;

    Node *left;
    Node *right;

    if(list->first->next == list->last) {
        cout << "Empty list on insert" << endl;
        right = list->last;
        left = list->first;
    } else {
        right = list->current->next;
        left = list->current;
    }

//    cout << "In insert" << endl;
//    cout << "Left pointer(current): " << left << endl;
//    cout << "Right pointer(current->next): " << list->current->next << endl;

    // check for empty list
    if (list->first->next == list->last) {
        list->rightSize = 1;
    } else {
        list->leftSize += 1;
    }

    left->next = temp;
    temp->previous = left;
    temp->next = right;
    right->previous = temp;

    temp->data = new(std::nothrow) Type;
    if (temp->data == NULL) {
        cout << "ERROR : Not enough memory. EXIT !!!\n";
        exit(1);
    }

    temp->data->Consume(Item);

    list->current = temp;
}

// get a reference to the current item in the list
template<class Type>
Type * TwoWayList<Type>::Current(int offset) {
    cout << "Current call offset: " << offset << endl;
    Node *temp = list->current;
    for (int i = 0; i < offset; i++) {
//        cout << "In current loop" << endl;
        temp = temp->next;
        if(temp == list->last) {
            return NULL;
        }
    }
    return temp->data;
}

// remove an item from the current position
template<class Type>
void
TwoWayList<Type>::Remove(Type *Item) {

    Node *temp = list->current;
    Node *prev = list->current->previous;
    Node *next = list->current->next;

    // if list becomes empty point current to first node
    if (next == list->last && prev == list->first) {
        list->current = list->first;
        list->rightSize = 0;
        list->leftSize = 0;
    } else if(next == list->last) {
        list->current = prev;
        (list->leftSize)--;
    } else {
        list->current = next;
        (list->rightSize)--;
    }

    Item->Consume(temp->data);

    delete temp;

    prev->next = next;
    next->previous = prev;
}

template <class Type>
bool
TwoWayList<Type>::IsListEmpty() {
    return list->first->next == list->last;
}

#endif
	
