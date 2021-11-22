//
// The file represents the set accept concurrent writing, and allows for non-blocking write
//

#ifndef PRACTICAL_1_NBSET_H
#define PRACTICAL_1_NBSET_H

#endif //PRACTICAL_1_NBSET_H

#include <stdlib.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// A Linked List style None-blocked set series,
typedef struct NBSet {
    struct NBSet *next;
    void *data;
} NBSet;

// Generate a set and return the address value of the set.
NBSet *create_N_set();

// Add a none-blocking object to the set.
int add_N_object(NBSet *set, void *object);

// Remove a none-blocking object from the set.
int remove_N_object(NBSet *set, void *object);

// Remove a none-blocking object from the set with index.
int remove_N_object_ind(NBSet *set, int ind);

// Check whether the linked list contains a specific object.
int contains_N_object(NBSet *set, void *object);

// Check the size of the linked list.
int N_sizeof(NBSet *set);