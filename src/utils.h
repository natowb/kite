#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Define the dynamic array struct with size and capacity fields
#define DEFINE_DYNAMIC_ARRAY(type) \
    typedef struct {               \
        type *data;                \
        size_t size;               \
        size_t capacity;           \
    } da_##type;

// Macro to initialize the dynamic array
#define INIT_DYNAMIC_ARRAY(arr, initial_capacity)     \
    do {                                              \
        (arr)->data = malloc((initial_capacity) * sizeof(*(arr)->data)); \
        (arr)->size = 0;                              \
        (arr)->capacity = initial_capacity;           \
    } while (0)

// Macro to resize the array if needed
#define RESIZE_DYNAMIC_ARRAY(arr, new_capacity)       \
    do {                                              \
        if ((arr)->capacity < (new_capacity)) {       \
            (arr)->capacity = (new_capacity);         \
            (arr)->data = realloc((arr)->data, (arr)->capacity * sizeof(*(arr)->data)); \
        }                                             \
    } while (0)

// Macro to push a new element into the dynamic array
#define PUSH_DYNAMIC_ARRAY(arr, value)                \
    do {                                              \
        if ((arr)->size == (arr)->capacity) {         \
            RESIZE_DYNAMIC_ARRAY(arr, (arr)->capacity * 2); \
        }                                             \
        (arr)->data[(arr)->size++] = (value);         \
    } while (0)

// Macro to free the dynamic array
#define FREE_DYNAMIC_ARRAY(arr)                       \
    do {                                              \
        free((arr)->data);                            \
        (arr)->data = NULL;                           \
        (arr)->size = 0;                              \
        (arr)->capacity = 0;                          \
    } while (0)


#endif // UTILS_H
