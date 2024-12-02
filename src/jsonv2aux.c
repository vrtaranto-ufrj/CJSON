#include <stdio.h>
#include <string.h>

#include "jsonv2aux.h"

JsonKeyValuePair * findKeyValuePair(JsonObject *json_obj, const char *key) {    
    for (JsonKeyValuePair *key_value_pair = json_obj->first; key_value_pair != NULL && key_value_pair; key_value_pair = key_value_pair->next) {
        if (strcmp(key_value_pair->key, key) == 0) {
            return key_value_pair;
        }
    }

    return NULL;
}

void setKeyValuePair(JsonObject *json_obj, const char *key, Json *value) {
    JsonKeyValuePair *old_json_key_value_pair = findKeyValuePair(json_obj, key);
    if (old_json_key_value_pair == NULL) {
        addKeyValuePair(json_obj, key, value);
        return;
    }

    // freeValue()  TEM QUE IMPLEMENTAR ISSO AQUI
    free(old_json_key_value_pair->value);

    old_json_key_value_pair->value = value;
}

void setArrayValue(JsonArray *json_array, size_t index, Json *value) {
    if (index >= json_array->array_size ) {
        fprintf(stderr, "Element, not inserted! Out of bounds");
        return;
    }

    json_array->array[index] = value;
}

void addKeyValuePair(JsonObject *json_obj, const char *key, Json *value) {
    JsonKeyValuePair * key_value_pair = (JsonKeyValuePair *) malloc(sizeof(JsonKeyValuePair));
    if (key_value_pair == NULL) {
        perror("malloc");
        fprintf(stderr, "Failed to create JsonKeyValuePair\n");
        return;
    }

    size_t key_size = strlen(key) + 1;
    key_value_pair->key = (char *) malloc(sizeof(char) * key_size);
    if (key_value_pair->key == NULL) {
        perror("malloc");
        fprintf(stderr, "Failed to aloc key string\n");
        return;
    }
    
    strcpy(key_value_pair->key, key);
    key_value_pair->value = value;
    key_value_pair->next = NULL;
    key_value_pair->previous = json_obj->last;

    if (json_obj->last == NULL) {
        json_obj->first = key_value_pair;
        json_obj->last = key_value_pair;
    } else {
        json_obj->last->next = key_value_pair;
        json_obj->last = key_value_pair;
    }
}

void setNull(JsonValue *json) {
    memset(json, 0, sizeof(JsonValue));
}

void setInt(JsonValue *json, int64_t val_int) {
    json->type = JSON_INT;
    json->value.val_int = val_int;
}

void setDouble(JsonValue *json, double val_double) {
    json->type = JSON_DOUBLE;
    json->value.val_double = val_double;
}

void setBool(JsonValue *json, bool val_bool) {
    json->type = JSON_BOOL;
    json->value.val_bool = val_bool;
}

void setString(JsonValue *json, char * val_string) {
    size_t string_len = strlen(val_string) + 1;

    json->value.val_string = (char *) malloc(string_len);
    if (json->value.val_string == NULL) {
        perror("malloc");
        fprintf(stderr, "Error setting string\n");
        return;
    }

    strcpy(json->value.val_string, val_string);
    json->type = JSON_STRING;
}

void setObject(JsonValue *json, JsonObject *obj_ptr) {
    json->type = JSON_OBJECT;
    json->value.object_ptr = obj_ptr;
}

void setArray(JsonValue *json, JsonArray *array_ptr) {
    json->type = JSON_ARRAY;
    json->value.array_ptr = array_ptr;
}