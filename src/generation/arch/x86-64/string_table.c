#include <generation/arch/x86-64/string_table.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/util.h>
#include <util/arraylist.h>

struct _StringTable {
    // ArrayList of StringTableItem --> ArrayList<StringTableItem> 
    ArrayList *string_items;
};

typedef struct _StringTableItem {
    // the string literal
    const char *string;

    // the additional labels for the string literal
    ArrayList *labels;
} StringTableItem;

StringTableItem *string_table_item_new(char *string);
void string_table_item_free(StringTableItem *item);

StringTableItem *string_table_item_new(char *string) {
    StringTableItem *item = malloc(sizeof(StringTableItem));
    item->string = string;
    item->labels = arraylist_create();
    return item;
}

void string_table_item_free(StringTableItem *item) {
    if (item->labels != NULL) {
        for (int i = 0; i < item->labels->size; i++) {
            free(item->labels->data[i]);
        }
        arraylist_free(item->labels);
    }
    free(item);
}

StringTable *string_table_new() {
    StringTable *table = calloc(1, sizeof(StringTable));
    table->string_items = arraylist_create();
    return table;
}

void string_table_free(StringTable *table) {
    if (table->string_items != NULL) {
        for (int i = 0; i < table->string_items->size; i++) {
            StringTableItem *item = arraylist_get(table->string_items, i);
            string_table_item_free(item);
        }
        arraylist_free(table->string_items);
    }
    free(table);
}

void string_table_add(StringTable *table, char *str) {
    if (string_table_contains(table, str)) {
        return;
    }

    StringTableItem *item = string_table_item_new(str);
    arraylist_add(table->string_items, item);
}

void string_table_add_label(StringTable *table, char *str, char *label) {
    int index = string_table_getIndex(table, str);

    if (index == -1) {
        return;
    }

    StringTableItem *item = arraylist_get(table->string_items, index);
    arraylist_add(item->labels, strdup(label));
}

int string_table_contains(StringTable *table, const char *str) {
    return string_table_getIndex(table, str) >= 0;
}

int string_table_getIndex(StringTable *table, const char *str) {
    if (table == NULL) return -1;
    if (str == NULL) return -1;

    for (size_t i = 0; i < table->string_items->size; i++) {
        StringTableItem *item = arraylist_get(table->string_items, i);
        if (strcmp(item->string, str) == 0) {
            return i;
        }
    }

    return -1;
}

char *string_table_toLNCIdentifier(StringTable *table, char *str) {
    if (table == NULL) return NULL;
    if (str == NULL) return NULL;

    int index = string_table_getIndex(table, str);

    if (index == -1) {
        string_table_add(table, str);
        index = string_table_getIndex(table, str);
    }

    const char *format = "_string_%d";
    size_t len = snprintf(NULL, 0, format, index) + 1;
    char *identifier = calloc(len, sizeof(char));
    sprintf(identifier, format, index);

    return identifier;
}

char *string_table_toString(StringTable *table) {
    if (table == NULL) return NULL;
    if (table->string_items == NULL) return NULL;
    if (table->string_items->size == 0) return calloc(1, sizeof(char));

    size_t buffer_size = 1; // null terminated string

    const char *label_format = "%s:\n";
    const char *string_format = "_string_%d: db `%s`,0\n";

    // calculate the size of the buffer
    for (size_t i = 0; i < arraylist_size(table->string_items); i++) {
        StringTableItem *item = arraylist_get(table->string_items, i);

        for (size_t i = 0; i < arraylist_size(item->labels); i++) {
            char *label = arraylist_get(item->labels, i);
            buffer_size += snprintf(NULL, 0, label_format, label);
        }

        buffer_size += snprintf(NULL, 0, string_format, i, item->string);
    }

    char *buffer = calloc(buffer_size, sizeof(char));
    int index = 0;

    // write to the buffer
    for (size_t i = 0; i < arraylist_size(table->string_items); i++) {
        StringTableItem *item = arraylist_get(table->string_items, i);
        
        for (size_t i = 0; i < arraylist_size(item->labels); i++) {
            char *label = arraylist_get(item->labels, i);
            index += snprintf(buffer + index, buffer_size - index, label_format, label);
        }

        index += snprintf(buffer + index, buffer_size - index, string_format, i, item->string);
    }

    return buffer;
}
