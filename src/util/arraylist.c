#include <util/arraylist.h>

#include <string.h>
#include <util/logging/logger.h>

ArrayListPtr arraylist_create() {
    ArrayList *list = malloc(sizeof *list);
    if (list == NULL) {
        return NULL;
    }

    list->size = 0;
    list->capacity = INITIAL_BASE_ARRAY_SIZE;
    list->data = calloc(list->capacity, sizeof(void *));
    if (list->data == NULL) {
        free(list); // Don't leek memory here!
        return NULL;
    }

    return list;
}

// ---------------------------------------------------------------------

size_t arraylist_size(ArrayList *list) {
    return list->size;
}

// ---------------------------------------------------------------------

size_t arraylist_capacity(ArrayList *list) {
    return list->capacity;
}

// ---------------------------------------------------------------------

void arraylist_add(ArrayList *arr, void *item) {
    while (arr->size >= arr->capacity) {
        // printf("REALLOCATING\n");
        arr->capacity += INITIAL_BASE_ARRAY_SIZE;
        arr->data = realloc(arr->data, sizeof(void *) * arr->capacity);
    }
    arr->data[arr->size++] = item;
}

// ---------------------------------------------------------------------

void arraylist_addall(ArrayList *arr, ArrayList *other) {
	// TODO: optimize with memcpy or something similar
	// for now this is good enough
	for (size_t i = 0; i < other->size; i++) {
		arraylist_add(arr, arraylist_get(other, i));
	}
}

// ---------------------------------------------------------------------

void *arraylist_get(ArrayList *arr, int index) {
    return arr->data[index];
}

// ---------------------------------------------------------------------

int arraylist_getindexof(ArrayList *arr, void *item) {
    for(int i = 0; i < arr->size; ++i)
        if (item == arraylist_get(arr, i))
            return i;
    return -1;
}

// ---------------------------------------------------------------------

void arraylist_set_at_index(ArrayList *list, int index, void *item, int _free) {
    if (index >= list->size) {
        printf("ERROR: Index out of Bounds: %d\n", index);
        return;
    }

    if (_free) 
        free(arraylist_get(list, index));
    
    list->data[index] = item;
}

// ---------------------------------------------------------------------

void arraylist_remove_at_index(ArrayList *arr, int index) {
    if (index > arr->size - 1) {
        printf("ERROR: Index out of Bounds: %d\n", index);
        return;
    }

    if (arr->size == 1) {
        arraylist_clear(arr);
        return;
    }

    // free(arraylist_get(arr, index));

    for (int i = index; i < arr->size; ++i) {
        if (i == arr->size - 1) {
            arr->data[i] = NULL;
        } else {
            arr->data[i] = arr->data[i + 1];
        }
    }
    arr->size--;
}

// ---------------------------------------------------------------------

ArrayList *arraylist_copy(ArrayList *src) {
	if (src == NULL) {
		log_warning("arraylist is NULL, not cloning arraylist!\n");
		return NULL;
	}
	ArrayList *copy = arraylist_create();
	copy->capacity = src->capacity;
	copy->size = src->size;
	memcpy(copy->data, src->data, sizeof(void *) * src->capacity);
	return copy;
}

// ---------------------------------------------------------------------

void arraylist_clear(ArrayList *arr) {
    arr->size = 0;
    arr->capacity = INITIAL_BASE_ARRAY_SIZE;
    free(arr->data);
    arr->data = NULL;
}

// ---------------------------------------------------------------------

void arraylist_shrink_capacity_to_size(ArrayList *arr) {
    arr->capacity = arr->size + 1;
    arr->data = realloc(arr->data, sizeof(void *) * arr->capacity);
}

// ---------------------------------------------------------------------

void arraylist_free(ArrayList *list) {
    if (list == NULL) {
        printf("ERROR: Arraylist is NULL!\n");
        return;
    }

    arraylist_clear(list);
    free(list);
}