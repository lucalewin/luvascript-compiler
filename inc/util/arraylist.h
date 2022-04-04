#ifndef LUVA_ARRAYLIST_H
#define LUVA_ARRAYLIST_H

#define INITIAL_BASE_ARRAY_SIZE 16

#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------

typedef struct _ArrayList ArrayList;
typedef ArrayList* ArrayListPtr;

// -----------------------------------------

struct _ArrayList {
	int capacity;
	size_t size;
	void **data;
};

// -----------------------------------------

ArrayList *arraylist_create();
size_t arraylist_size(ArrayList *list);
size_t arraylist_capacity(ArrayList *list);
void arraylist_add(ArrayList *arr, void *item);
void arraylist_addall(ArrayList *arr, ArrayList *other);
void *arraylist_get(ArrayList *arr, int index);
int arraylist_getindexof(ArrayList *arr, void *item);
void arraylist_set_at_index(ArrayList *list, int index, void *item, int _free);
void arraylist_remove_at_index(ArrayList *arr, int index);
ArrayList *arraylist_copy(ArrayList *src);
void arraylist_clear(ArrayList *arr);
void arraylist_shrink_capacity_to_size(ArrayList *arr);
void arraylist_free(ArrayList *list);

// -----------------------------------------

#endif // LUVA_ARRAYLIST_H