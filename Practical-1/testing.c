// Header of this file
#include "testing.h"

// System lib
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Tested set
#include "BSet.h"
#include "NBSet.h"
#include "HashSet.h"

// The global variable being used for operating
BSet *bSet;
NBSet *nSet;
HashSet *hSet;

// Alert for current running index
int finished = 1;

void status_check(int status) {
    // 0 - false, 1 true
    if (status != 0 && status != 1) {
        exit(status);
    }
}

void t_100_B_add(void *ptr) {
    for (int i = 0; i < *((int*)ptr); i++) {
        int val = rand();
        status_check(add_B_object(bSet, &val));
//        printf("Size: %i\n", bSet->size);
    }
}

void t_100_N_add(void *ptr) {
    for (int i = 0; i < *((int*)ptr); i++) {
        int val = rand();
        status_check(add_N_object(nSet, &val));
    }
}

void t_100_H_add(void *ptr) {
    for (int i = 0; i < *((int*)ptr); i++) {
        int val = rand();
//        printf("%i\n", val);
        status_check(add_H_node(hSet, &val));
    }
}

void counting(void* func) {
    int num = 20;

    void *retVal;

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;

    long current = clock();

//    printf("%lu", func);
//    return;
    pthread_create(&thread1, NULL, func, &num);
    pthread_create(&thread2, NULL, func, &num);
    pthread_create(&thread3, NULL, func, &num);

    pthread_join(thread1, &retVal);
    pthread_join(thread2, &retVal);
    pthread_join(thread3, &retVal);

    long t_cost = clock() - current;
    printf("Time cost for current set is %li\n", t_cost);
    // Show print configuration to next function
    finished = 0;
}

// Initialize properties for global variables &
void init_testing() {
    bSet = create_B_set();
    nSet = create_N_set();
    hSet = create_hash_set();
}

// Testing for Space cost for different sets.
void testSpaceCost() {

}

void add_test_data_H(int test_num) {
    status_check(add_H_node(hSet, &test_num));
}

void test_H() {
    int test_num = 1000;
    int invalid_num = 2000;
    counting((void*)&t_100_H_add);
    while (finished == 1);
    finished = 1;
    long current = clock();
    add_test_data_H(test_num);
    printf("Invalid check: 2000: %i\n", contains_H_node(hSet, &invalid_num));
    printf("Contain check: 1000: %i\n", contains_H_node(hSet, &test_num));
    remove_H_node(hSet, &test_num);
    printf("Node removed for 1000.\n");
    printf("Contain check: 1000: %i\n", contains_H_node(hSet, &test_num));
    printf("The process costs %li for removing and containing test\n", clock() - current);
    destroy_all_H(hSet);
}

void test_N() {
    int test_num = 1000;
    int invalid_num = 2000;
    counting((void*)t_100_N_add);
    while (finished == 1);
    finished = 1;
}

void test_B() {
    int test_num = 10000;
    int invalid_num = 2000;
    counting((void*)t_100_B_add);
    // Reset the list.
    while (finished == 1);
    finished = 1;
    int status_code;
    add_B_object(bSet, &test_num);
    long current = clock();
    printf("Adding object %i to the list...\n", test_num);
    status_code = contains_B_object(bSet, &test_num);
    printf("Checking containing the test number: %i... status %i\n", test_num, status_code);
    status_code = contains_B_object(bSet, &invalid_num);
    printf("Checking containing the test number: %i... status %i\n", invalid_num, status_code);
    status_code = remove_B_object(bSet, &test_num);
    printf("Checking the validation of object %i removal: %i\n", test_num , status_code);
    status_code = remove_B_object(bSet, &invalid_num);
    printf("Checking the validation of object %i removal: %i\n", invalid_num , status_code);
    printf("The process costs %li for removing and containing test\n", clock() - current);
    add_B_object(bSet, &test_num);
    add_B_object(bSet, &invalid_num);
    remove_B_object(bSet, &invalid_num);
    remove_B_object(bSet, &test_num);
    destroy_all_BNode(bSet);
}

// Testing the time cost for different sets.
void testTimeCost() {
    test_B();
//    test_H();
//    test_N();
}

