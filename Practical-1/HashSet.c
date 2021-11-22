//
// The implementation of the hash set.
//
#include "HashSet.h"

HashSet *create_hash_set() {
    // Memory allocation for the set
    HashSet *set = (HashSet*)malloc(sizeof(HashSet));
    // 10 20 40 80 / Assign the start of the array to the hash set
    set -> max_size = 20;
    set -> addrList = (unsigned long*)calloc(set->max_size, sizeof(unsigned long));
    set -> size = 0;
    // Initialize the mutex lock and condition
    pthread_mutex_init(&set->plock, NULL);
    pthread_cond_init(&set->pcond, NULL);
    // Return the pointer for the set.
    return set;
}

int add_H_node(HashSet *set, void *data) {
    // Malloc a space for storing data.
    HNode *node = (HNode*)malloc(sizeof(HNode));
    if (node == NULL) {
        printf("No enough memory space!\n");
        return -255;
    }
    // Since the function is exported, we can directly use the function.
    if (contains_H_node(set, data)) {
        return 1;
    }
    node->data = *(int*)data;

    // Lock the list: With mutex
    pthread_mutex_lock(&set->plock);
    // Take 10 free mem space ready for concurrency.
    // Memory are initialized and copied and set 0 with calloc.
    if (set -> max_size < set -> size + 10) {
        // Double current memory space for flexible use.
        set -> max_size = (set -> max_size) * 2;
        unsigned long* tmp = (unsigned long*)calloc(set->max_size, sizeof(unsigned long));
        if (tmp == NULL) {
            printf("No enough memory space!\n");
            return -255;
        }
        memcpy(tmp, set->addrList, sizeof(set->addrList) * (set->max_size / 2));
        free(set->addrList);
        set->addrList = tmp;
    }

    // Read the address list and find the address space that is 0.
    int i = 0;
    while (*((unsigned long*)(set->addrList + (i * sizeof(unsigned long)))) != 0) {
//        printf("%lu - %i\n", *((unsigned long*)(set->addrList + (i * sizeof(unsigned long)))), i);
        ++i;
    }
    // Assign elements to the specified position of the pointer.
    *(set -> addrList + (i * sizeof(unsigned long))) = (unsigned long)node;
    // Increment the value of the size.
    ++(set->size);

    pthread_mutex_unlock(&set->plock);

    //     Debugger:
//    printf("Node address: %lu %lx / %i, Array address: %lu, %lu, %i / %lx, Array: %lu, Set size: %i / %i\n",
//           (unsigned long)node, (unsigned long)node, node->data,
//           (unsigned long)(set->addrList + (i * sizeof(unsigned long))),
//           *(unsigned long*)(set->addrList + (i * sizeof(unsigned long))),
//           ((HNode*)(*(unsigned long*)(set->addrList + (i * sizeof(unsigned long)))))->data,
//           *(unsigned long*)(set->addrList + (i * sizeof(unsigned long))),
//           (unsigned long) set->addrList, set->size, set->max_size);

    return 0;
}

// Remove the node with specific integer.
int remove_H_node(HashSet *set, void *data) {
    for (int i = 0; i < set->max_size; i++) {
        // Prevent read/write concurrency
        pthread_mutex_lock(&set->plock);
        unsigned long address = *(set->addrList + i * sizeof(unsigned long));
        if (address > 0) {
//            int retrieved = ((HNode*)(*(unsigned long*)(set->addrList + (i * sizeof(unsigned long)))))->data;
            int retrieved = ((HNode*)address)->data;
//            printf("%i is passed to contains check with address %lu, checking NO. %i element - %i at position %lu / %lu currently.\n",
//                   *(int*)data, (unsigned long)data, i, retrieved, address,
//                   (unsigned long)(set->addrList + (i * sizeof(unsigned long))));
            if (retrieved == *(int*)data) {
                free((HNode*)address);
                *(set->addrList + i * sizeof(unsigned long)) = 0;
                --set->size;
                pthread_mutex_unlock(&set->plock);
                return 0;
            }
        }
        pthread_mutex_unlock(&set->plock);
    }
    return 404; // Node not exists
}

// Similar to remove the node, contains directly return 1 if the node contains the value, and 0 otherwise,
int contains_H_node(HashSet *set, void *data) {
//    printf("%i is passed to contains check with address %lu\n", *(int*)data, data);
//    return 0;
    for (int i = 0; i < set->max_size; i++) {
        // Prevent read/write concurrency
        pthread_mutex_lock(&set->plock);
//        printf("%i \n", ((HNode*)*(set->addrList + i * sizeof(unsigned long)))->data);
        unsigned long address = *(set->addrList + i * sizeof(unsigned long));
        if (address > 0) {
//            int retrieved = ((HNode*)(*(unsigned long*)(set->addrList + (i * sizeof(unsigned long)))))->data;
            int retrieved = ((HNode*)address)->data;
//            printf("%i is passed to contains check with address %lu, checking NO. %i element - %i at position %lu / %lu currently.\n",
//                   *(int*)data, (unsigned long)data, i, retrieved, address,
//                   (unsigned long)(set->addrList + (i * sizeof(unsigned long))));
            if (retrieved == *(int*)data) {
                pthread_mutex_unlock(&set->plock);
                return 1;
            }
        }
//        printf("Break point: %lu, %i\n", *(set->addrList + (i * sizeof(unsigned long))), i);
        pthread_mutex_unlock(&set->plock);
    }
    return 0;
}

// Free all elements in set H.
void destroy_all_H(HashSet *set) {
//    int i = 0;
//    while ((set->addrList + i * sizeof(unsigned long)) > 0) {
//        printf("%i\n", ((HNode*)(*(unsigned long*)(set->addrList + (i * sizeof(unsigned long)))))->data);
//        free((HNode*)(*(unsigned long*)(set->addrList + (i * sizeof(unsigned long)))));
//        ++i;
//    }
    int i = 0;
    while (i < set->max_size) {
//        printf("%i\n", ((HNode*)(*(unsigned long*)(set->addrList + (i * sizeof(unsigned long)))))->data);
        if(*((unsigned long*)set->addrList + (i * sizeof(unsigned long))) > 0) {
            free((HNode*)(*(unsigned long*)(set->addrList + (i * sizeof(unsigned long)))));
        }
        ++i;
    }
    free(set->addrList);
    free(set);
}


