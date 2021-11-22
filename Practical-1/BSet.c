#include "BSet.h"

BSet *create_B_set() {
    // Create a header for the set.
    BSet *header = malloc(sizeof(BSet));
    header -> root = NULL;
    header -> size = 0;
    pthread_mutex_init(&header->plock, NULL);
    return header;
}

// Add an object to the set.
int add_B_object(BSet *set, void *object) {
    if (contains_B_object(set, object)) {
        return 1;
    }
    // Create the Node for the object. This step runs concurrently.
    BNode *node = (BNode*)malloc(sizeof(BNode));
    node->data = *(int*)object;
    node->next = NULL;
    node->prev = NULL;
    pthread_mutex_init(&node->plock, NULL);
    // Find the last object. Adding to set runs concurrently.

    // The first object runs
    pthread_mutex_lock(&set -> plock);
    if (set -> size == 0) {
        set -> root = node;
        ++set->size;
        pthread_mutex_unlock(&set -> plock);
        return 0;
    }
    pthread_mutex_unlock(&set -> plock);

    // Assign the root object pointer to the temporary node.
    // Searching should prevent from writing, add a lock to the block.
    BNode *tmp = set->root;
    // Infinite loop runs for preventing unnecessary lock
    while (1){
        BNode *block = tmp;
        // Lock the single node for efficiency.
        pthread_mutex_lock(&block->plock);
        if (tmp -> next != NULL) {
            tmp = tmp -> next;
        } else {
            tmp -> next = node;
            node -> prev = tmp;
            ++set->size;
            pthread_mutex_unlock(&block->plock);
            break;
        }
        pthread_mutex_unlock(&block->plock);
    }
    return 0;
}

int remove_B_object(BSet *set, void *object) {
    // Empty condition
    pthread_mutex_lock(&set->plock);
    if (set -> root == NULL) {
        return 404;
    }
    pthread_mutex_unlock(&set->plock);
    // Add lock to previous, next, current
    int data = *(int*)object;
    BNode *tmp = set->root;
    BNode *delete;
    // If the first node is the data we would delete
    if (tmp->data == data) {
        delete = tmp;
        // Unbind the deleted node from other node, lock the next node and deleted node.
        pthread_mutex_lock(&delete->plock);
        if (delete->next != NULL) {
            BNode *next = delete -> next;
            // Assign value to set.
            pthread_mutex_lock(&set->plock);
            set -> root = delete -> next;
            --set -> size;
            pthread_mutex_unlock(&set->plock);

            pthread_mutex_lock(&next -> plock);
            next -> prev = NULL;
            pthread_mutex_unlock(&next -> plock);
        } else {
            // Assign value to set.
            pthread_mutex_lock(&set->plock);
            set -> root = NULL;
            --set -> size;
            pthread_mutex_unlock(&set->plock);
        }
        pthread_mutex_unlock(&delete->plock);
        free(delete);
        --set->size;
        return 0;
    }
    // Condition for medium nodes
    while (1) {
        // Locker locks the previous node
        BNode *prev = tmp;
        pthread_mutex_lock(&prev->plock);
        if (tmp->next != NULL) {
            tmp = tmp -> next;
            pthread_mutex_lock(&tmp->plock);
            if (tmp -> data == data) {
                prev -> next = tmp -> next;
                // Condition for last node.
                if (tmp -> next != NULL) {
                    BNode *next = tmp -> next;
                    pthread_mutex_lock(&next -> plock);
                    next -> prev = prev;
                    pthread_mutex_unlock(&next -> plock);
                }
                pthread_mutex_unlock(&prev -> plock);
                pthread_mutex_unlock(&tmp->plock);
                free(tmp);
                --set -> size;
                return 0;
            }
            pthread_mutex_unlock(&tmp->plock);
        }
        else {
            pthread_mutex_unlock(&prev -> plock);
            return 404;
        }
        pthread_mutex_unlock(&prev -> plock);
    }
}

// Check whether the node contains something passed into.
int contains_B_object(BSet *set, void *object) {
    pthread_mutex_lock(&set->plock);
    if (set->root == NULL) {
        pthread_mutex_unlock(&set->plock);
        return 0;
    }
    BNode *node = set->root;
    pthread_mutex_unlock(&set->plock);

    int data = *(int*)object;
    while (1) {
        pthread_mutex_lock(&node -> plock);
        if (node -> data == data) {
            pthread_mutex_unlock(&node->plock);
            return 1;
        }

        if (node -> next != NULL) {
//            printf("data: %i / %i\n", data, node->data);
            node = node->next;
        } else {
            pthread_mutex_unlock(&node->plock);
            return 0;
        }
        pthread_mutex_unlock(&node->prev->plock);
    }
}

int B_sizeof(BSet *set) {
    return set->size;
}

void destroy_all_BNode(BSet *set) {

}