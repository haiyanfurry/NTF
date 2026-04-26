#define _GNU_SOURCE

#include "data_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 32
#define GROWTH_FACTOR 2

static void free_key_value_pair(KeyValuePair* pair);
static int find_key_index(DataManager* manager, const char* key);
static bool grow_capacity(DataManager* manager);

static void free_key_value_pair(KeyValuePair* pair) {
    if (pair) {
        if (pair->key) free(pair->key);
        if (pair->value) free(pair->value);
        free(pair);
    }
}

static int find_key_index(DataManager* manager, const char* key) {
    if (!manager || !key) {
        return -1;
    }
    
    for (size_t i = 0; i < manager->item_count; i++) {
        if (strcmp(manager->items[i].key, key) == 0) {
            return (int)i;
        }
    }
    
    return -1;
}

static bool grow_capacity(DataManager* manager) {
    if (!manager) {
        return false;
    }
    
    size_t new_capacity = (manager->item_capacity == 0) ? INITIAL_CAPACITY : manager->item_capacity * GROWTH_FACTOR;
    KeyValuePair* new_items = (KeyValuePair*)realloc(manager->items, sizeof(KeyValuePair) * new_capacity);
    
    if (!new_items) {
        fprintf(stderr, "Error growing data manager capacity\n");
        return false;
    }
    
    manager->items = new_items;
    manager->item_capacity = new_capacity;
    
    return true;
}

DataManager* data_manager_init(DataStorageType type, const char* storage_path) {
    DataManager* manager = (DataManager*)malloc(sizeof(DataManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory for data manager\n");
        return NULL;
    }
    
    manager->storage_type = type;
    manager->storage_context = storage_path ? strdup(storage_path) : NULL;
    manager->items = NULL;
    manager->item_count = 0;
    manager->item_capacity = 0;
    
    if (!grow_capacity(manager)) {
        free(manager->storage_context);
        free(manager);
        return NULL;
    }
    
    return manager;
}

void data_manager_destroy(DataManager* manager) {
    if (manager) {
        for (size_t i = 0; i < manager->item_count; i++) {
            if (manager->items[i].key) free(manager->items[i].key);
            if (manager->items[i].value) free(manager->items[i].value);
        }
        if (manager->items) free(manager->items);
        if (manager->storage_context) free(manager->storage_context);
        free(manager);
    }
}

DataResult data_manager_save(DataManager* manager, const char* key, const void* value, size_t value_size) {
    if (!manager || !key || !value || value_size == 0) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    int index = find_key_index(manager, key);
    
    if (index >= 0) {
        if (manager->items[index].value) free(manager->items[index].value);
        
        manager->items[index].value = malloc(value_size);
        if (!manager->items[index].value) {
            return DATA_RESULT_MEMORY_ERROR;
        }
        
        memcpy(manager->items[index].value, value, value_size);
        manager->items[index].value_size = value_size;
    } else {
        if (manager->item_count >= manager->item_capacity) {
            if (!grow_capacity(manager)) {
                return DATA_RESULT_MEMORY_ERROR;
            }
        }
        
        manager->items[manager->item_count].key = strdup(key);
        manager->items[manager->item_count].value = malloc(value_size);
        
        if (!manager->items[manager->item_count].key || !manager->items[manager->item_count].value) {
            if (manager->items[manager->item_count].key) free(manager->items[manager->item_count].key);
            if (manager->items[manager->item_count].value) free(manager->items[manager->item_count].value);
            return DATA_RESULT_MEMORY_ERROR;
        }
        
        memcpy(manager->items[manager->item_count].value, value, value_size);
        manager->items[manager->item_count].value_size = value_size;
        manager->items[manager->item_count].value_type = 0;
        manager->item_count++;
    }
    
    return DATA_RESULT_SUCCESS;
}

DataResult data_manager_get(DataManager* manager, const char* key, void** value, size_t* value_size) {
    if (!manager || !key || !value) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    int index = find_key_index(manager, key);
    
    if (index < 0) {
        return DATA_RESULT_NOT_FOUND;
    }
    
    *value = malloc(manager->items[index].value_size);
    if (!*value) {
        return DATA_RESULT_MEMORY_ERROR;
    }
    
    memcpy(*value, manager->items[index].value, manager->items[index].value_size);
    if (value_size) {
        *value_size = manager->items[index].value_size;
    }
    
    return DATA_RESULT_SUCCESS;
}

DataResult data_manager_delete(DataManager* manager, const char* key) {
    if (!manager || !key) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    int index = find_key_index(manager, key);
    
    if (index < 0) {
        return DATA_RESULT_NOT_FOUND;
    }
    
    if (manager->items[index].key) free(manager->items[index].key);
    if (manager->items[index].value) free(manager->items[index].value);
    
    for (size_t i = index; i < manager->item_count - 1; i++) {
        manager->items[i] = manager->items[i + 1];
    }
    
    manager->item_count--;
    
    return DATA_RESULT_SUCCESS;
}

bool data_manager_has(DataManager* manager, const char* key) {
    return find_key_index(manager, key) >= 0;
}

DataResult data_manager_clear(DataManager* manager) {
    if (!manager) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < manager->item_count; i++) {
        if (manager->items[i].key) free(manager->items[i].key);
        if (manager->items[i].value) free(manager->items[i].value);
    }
    
    manager->item_count = 0;
    
    return DATA_RESULT_SUCCESS;
}

char** data_manager_get_all_keys(DataManager* manager, size_t* count) {
    if (!manager || !count) {
        return NULL;
    }
    
    *count = manager->item_count;
    
    if (manager->item_count == 0) {
        return NULL;
    }
    
    char** keys = (char**)malloc(sizeof(char*) * manager->item_count);
    if (!keys) {
        return NULL;
    }
    
    for (size_t i = 0; i < manager->item_count; i++) {
        keys[i] = strdup(manager->items[i].key);
    }
    
    return keys;
}

DataResult data_manager_save_string(DataManager* manager, const char* key, const char* value) {
    if (!value) {
        return DATA_RESULT_INVALID_PARAM;
    }
    return data_manager_save(manager, key, value, strlen(value) + 1);
}

DataResult data_manager_get_string(DataManager* manager, const char* key, char** value) {
    if (!value) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    size_t size;
    DataResult result = data_manager_get(manager, key, (void**)value, &size);
    return result;
}

DataResult data_manager_save_int(DataManager* manager, const char* key, int value) {
    return data_manager_save(manager, key, &value, sizeof(int));
}

DataResult data_manager_get_int(DataManager* manager, const char* key, int* value) {
    if (!value) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    size_t size;
    return data_manager_get(manager, key, (void**)&value, &size);
}

DataResult data_manager_save_double(DataManager* manager, const char* key, double value) {
    return data_manager_save(manager, key, &value, sizeof(double));
}

DataResult data_manager_get_double(DataManager* manager, const char* key, double* value) {
    if (!value) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    size_t size;
    return data_manager_get(manager, key, (void**)&value, &size);
}

DataResult data_manager_save_bool(DataManager* manager, const char* key, bool value) {
    return data_manager_save(manager, key, &value, sizeof(bool));
}

DataResult data_manager_get_bool(DataManager* manager, const char* key, bool* value) {
    if (!value) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    size_t size;
    return data_manager_get(manager, key, (void**)&value, &size);
}

DataResult data_manager_flush(DataManager* manager) {
    if (!manager || manager->storage_type != DATA_STORAGE_FILE || !manager->storage_context) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    FILE* file = fopen((char*)manager->storage_context, "w");
    if (!file) {
        fprintf(stderr, "Error opening data file for writing\n");
        return DATA_RESULT_WRITE_ERROR;
    }
    
    for (size_t i = 0; i < manager->item_count; i++) {
        fprintf(file, "[%s]\n", manager->items[i].key);
        fprintf(file, "size=%zu\n", manager->items[i].value_size);
        fprintf(file, "type=%d\n", manager->items[i].value_type);
        fprintf(file, "value=");
        for (size_t j = 0; j < manager->items[i].value_size; j++) {
            fprintf(file, "%02x", ((unsigned char*)manager->items[i].value)[j]);
        }
        fprintf(file, "\n\n");
    }
    
    fclose(file);
    printf("Data flushed to: %s\n", (char*)manager->storage_context);
    
    return DATA_RESULT_SUCCESS;
}

DataResult data_manager_load(DataManager* manager) {
    if (!manager || manager->storage_type != DATA_STORAGE_FILE || !manager->storage_context) {
        return DATA_RESULT_INVALID_PARAM;
    }
    
    FILE* file = fopen((char*)manager->storage_context, "r");
    if (!file) {
        fprintf(stderr, "Error opening data file for reading\n");
        return DATA_RESULT_READ_ERROR;
    }
    
    char line[1024];
    char current_key[256] = {0};
    size_t current_size = 0;
    int current_type = 0;
    unsigned char* current_value = NULL;
    
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[' && line[strlen(line)-2] == ']') {
            strncpy(current_key, line + 1, strlen(line) - 3);
            current_key[strlen(line) - 3] = '\0';
        } else if (strncmp(line, "size=", 5) == 0) {
            current_size = atoll(line + 5);
        } else if (strncmp(line, "type=", 5) == 0) {
            current_type = atoi(line + 5);
        } else if (strncmp(line, "value=", 6) == 0) {
            if (current_size > 0) {
                current_value = (unsigned char*)malloc(current_size);
                if (current_value) {
                    char* hex = line + 6;
                    for (size_t i = 0; i < current_size; i++) {
                        sscanf(hex + i * 2, "%02hhx", &current_value[i]);
                    }
                    
                    data_manager_save(manager, current_key, current_value, current_size);
                    int index = find_key_index(manager, current_key);
                    if (index >= 0) {
                        manager->items[index].value_type = current_type;
                    }
                    
                    free(current_value);
                    current_value = NULL;
                }
            }
        }
    }
    
    fclose(file);
    printf("Data loaded from: %s\n", (char*)manager->storage_context);
    
    return DATA_RESULT_SUCCESS;
}
