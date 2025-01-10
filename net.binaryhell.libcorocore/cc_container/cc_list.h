#ifndef CC_LIST_H
#define CC_LIST_H

#include "cc_config.h"

typedef struct cc_list_s {
    struct cc_list_s* prev;
    struct cc_list_s* next;
} cc_list_t;

void cc_list_init( cc_list_t* list );
bool cc_list_empty( cc_list_t* list );
void cc_list_push_front( cc_list_t* list, cc_list_t* node );
void cc_list_push_back( cc_list_t* list, cc_list_t* node );
cc_list_t* cc_list_pop( cc_list_t* node );
cc_list_t* cc_list_pop_front( cc_list_t* list );
cc_list_t* cc_list_pop_back( cc_list_t* list );

#define cc_list_foreach( LIST, ITERATOR ) \
    for( cc_list_t* ITERATOR = (LIST)->next, *ITERATOR##_safe; ( ITERATOR != (LIST) ) && ( ITERATOR##_safe = ITERATOR->next ); ITERATOR = ITERATOR##_safe )

#define cc_list_foreach_back( LIST, ITERATOR ) \
    for( cc_list_t* ITERATOR = (LIST)->prev, *ITERATOR##_safe; ( ITERATOR != (LIST) ) && ( ITERATOR##_safe = ITERATOR->prev ); ITERATOR = ITERATOR##_safe )

#define cc_list_pop_container( LIST, TYPE, FIELD ) \
    cc_container_of( cc_list_pop( LIST ), TYPE, FIELD )

#define cc_list_pop_front_container( LIST, TYPE, FIELD ) \
    cc_container_of( cc_list_pop_front( LIST ), TYPE, FIELD )

#define cc_list_pop_back_container( LIST, TYPE, FIELD ) \
    cc_container_of( cc_list_pop_back( LIST ), TYPE, FIELD )

#endif
