//
// Created by Lenovo on 19/04/2021.
//

#include "parrlib.h"
#include "malloc.h"

// Queue will runs in a single thread, automatically pop and execute the task.
typedef struct Queue {
    long timestamp;  // The timestamp of the node
    void* func;      // The function pointer
    void* parameter; // The parameter passes to the function
    struct Queue *next;
} TaskQueue;

TaskQueue *root;

void makeQueue() {
    root = malloc(sizeof(TaskQueue));
}

void destroyQueue() {
    for ()
}

// The divider function of the library
void* divider() {

}

// The solver function of the library
void* solver() {

}

// The combiner function of the library
void* combiner() {

}