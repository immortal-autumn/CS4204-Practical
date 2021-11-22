// The file represents a Set that allows concurrent writing and blocked.
#ifndef PRACTICAL_1_BSET_H
#define PRACTICAL_1_BSET_H

#endif //PRACTICAL_1_BSET_H

#include <stdlib.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


// A classical blocked concurrent-supported set series. This prevents the condition of removing the first node.
typedef struct BNode {
    // Retrieving the data based on the use of the "size"
    struct BNode *next;
    struct BNode *prev;
    // Choose int type for easier comparison.
    int data;
    // Lock the single node enhance the concurrency.
    pthread_mutex_t plock;
//    pthread_cond_t pcond;
} BNode;

// A header is used for store the size of the list and more. Root node stores the object of the first node.
typedef struct BSet {
    struct BNode *root;
    int size;
    pthread_mutex_t plock;
//    pthread_cond_t pcond;
} BSet;

BSet *create_B_set();

// Add a none-blocking object to the set.
int add_B_object(BSet *set, void *object);

// Remove a none-blocking object from the set.
int remove_B_object(BSet *set, void *object);

// Check whether the linked list contains a specific object.
int contains_B_object(BSet *set, void *object);

// Check the size of the linked list.
int B_sizeof(BSet *set);

void destroy_all_BNode(BSet *set);