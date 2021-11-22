//
// This set generates a hashset for handling concurrences.
//

#ifndef PRACTICAL_1_HASHSET_H
#define PRACTICAL_1_HASHSET_H

#endif //PRACTICAL_1_HASHSET_H

#include <stdlib.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// The algorithm of HashSet saves address for Nodes and reads information from address space.
// This set is a Blocking set.
typedef struct HashSet {
    // Store the max_size of the address book.
    int max_size;
    // Store the size of the address list. This function can be used for "Store" function
    int size;
    // Store the start value of the address book
    unsigned long* addrList;
    // Locker
    pthread_mutex_t plock;
    pthread_cond_t pcond;
} HashSet;

// The structure of the hash node, which stores the data,
typedef struct HNode {
    int data;
} HNode;

// The initializer for the set
HashSet *create_hash_set();

// Functions for the set.
int add_H_node(HashSet *set, void *data);

int remove_H_node(HashSet *set, void *data);

int contains_H_node(HashSet *set, void *data);

int H_sizeof();

void destroy_all_H(HashSet *set);