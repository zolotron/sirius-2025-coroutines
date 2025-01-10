#include "cc_list.h"

void cc_list_init( cc_list_t* list ) {
    assert( list != NULL );
    list->prev = list;
    list->next = list;
}

bool cc_list_empty( cc_list_t* list ) {
    assert( list != NULL );
    return ( list->next == list );
}

void cc_list_push_front( cc_list_t* list, cc_list_t* node ) {
    assert( list != NULL );
    assert( node != NULL );
    node->prev = list;
    node->next = list->next;
    list->next->prev = node;
    list->next = node;
}

void cc_list_push_back( cc_list_t* list, cc_list_t* node ) {
    assert( list != NULL );
    assert( node != NULL );
    node->prev = list->prev;
    node->next = list;
    list->prev->next = node;
    list->prev = node;
}

cc_list_t* cc_list_pop( cc_list_t* node ) {
    assert( node != NULL );
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node;
    node->next = node;
    return node;
}

cc_list_t* cc_list_pop_front( cc_list_t* list ) {
    assert( list != NULL );
    return cc_list_pop( list->next );
}

cc_list_t* cc_list_pop_back( cc_list_t* list ) {
    assert( list != NULL );
    return cc_list_pop( list->prev );
}
