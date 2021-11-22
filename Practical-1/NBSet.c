#include <stdlib.h>
#include "NBSet.h"

NBSet *create_N_set() {
    NBSet *set = malloc(sizeof(NBSet));
    return set;
}

int add_N_object(NBSet *set, void *object) {
    return 0;
}

int remove_N_object(NBSet *set, void *object) {
    return 0;
}

int remove_N_object_ind(NBSet *set, int ind) {
    return 0;
}

int contains_N_object(NBSet *set, void *object) {
    return 0;
}

int N_sizeof(NBSet *set) {
    return 0;
}