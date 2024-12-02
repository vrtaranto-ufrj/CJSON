#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#include "jsonv2.h"
#include "jsonv2aux.h"

char * jsonfyRecursive(Json *json, char *json_string, size_t *string_len);
char * jsonfyNull(Json *json, char *json_string, size_t *string_len);
char * jsonfyInt(Json *json, char *json_string, size_t *string_len);
char * jsonfyDouble(Json *json, char *json_string, size_t *string_len);
char * jsonfyBool(Json *json, char *json_string, size_t *string_len);
char * jsonfyString(Json *json, char *json_string, size_t *string_len);
char * jsonfyObject(Json *json, char *json_string, size_t *string_len);
char * jsonfyArray(Json *json, char *json_string, size_t *string_len);


Json * createJson(JsonTypes type, void * value) {
    Json * new_json = (Json *) calloc(1, sizeof(Json));
    if (new_json == NULL) {
        perror("calloc");
        fprintf(stderr, "Failed to create Json\n");
        return NULL;
    }
    setJsonValueType(new_json, type, value);

    return new_json;
}

JsonObject * createJsonObject() {
    JsonObject * new_json_obj = (JsonObject *) calloc(1, sizeof(JsonObject));
    if (new_json_obj == NULL) {
        perror("calloc");
        fprintf(stderr, "Failed to create JsonObject\n");
        return NULL;
    }

    return new_json_obj;
}

JsonArray * createJsonArray(size_t array_size) {
    JsonArray * new_json_array = (JsonArray *) malloc(sizeof(JsonArray));
    if (new_json_array == NULL) {
        perror("malloc");
        fprintf(stderr, "Failed to create JsonArray\n");
        return NULL;
    }

    new_json_array->array = (JsonValue **) calloc(array_size, sizeof(JsonValue));
    if (new_json_array->array == NULL) {
        perror("calloc");
        fprintf(stderr, "Failed to create JsonValues from JsonArray\n");
        return NULL;
    } 

    new_json_array->array_size = array_size;

    return new_json_array;
}

void changeJsonArraySize(JsonArray *json_array, size_t array_size) {
    json_array->array = (JsonValue **) realloc(json_array->array, sizeof(JsonValue) * array_size);
    if (json_array->array == NULL) {
        perror("realloc");
        fprintf(stderr, "Failed to realloc JsonArray\n");
        return;
    }

    json_array->array_size = array_size;
}

void setJsonValueType(Json *json, JsonTypes type, void * value) {
    switch (type) {
    case JSON_NULL:
        setNull(json);
        break;
    case JSON_INT:
        setInt(json, *(int64_t*) value);
        break;
    case JSON_DOUBLE:
        setDouble(json, *(double*) value);
        break;
    case JSON_BOOL:
        setBool(json, *(bool*) value);
        break;
    case JSON_STRING:
        setString(json, (char*) value);
        break;
    case JSON_OBJECT:
        setObject(json, (JsonObject*) value);
        break;
    case JSON_ARRAY:
        setArray(json, (JsonArray*) value);
        break;
    default:
        fprintf(stderr, "Invalid JsonType\n");
        break;
    }
}

void freeJson(Json *json) {
    if (json == NULL) return;
    switch (json->type) {
    case JSON_NULL:
    case JSON_INT:
    case JSON_DOUBLE:
    case JSON_BOOL:
        break;
    case JSON_STRING:
        free(json->value.val_string);
        break;
    case JSON_OBJECT:
        freeJsonObject(json->value.object_ptr);
        break;
    case JSON_ARRAY:
        freeJsonArray(json->value.array_ptr);
        break;
    default:
        fprintf(stderr, "Invalid JsonType\n");
        break;
    }
    free(json);
}

void freeJsonObject(JsonObject *json_object) {
    for (JsonKeyValuePair *key_value_pair = json_object->first; key_value_pair != NULL; ) {
        JsonKeyValuePair *next_pair = key_value_pair->next;
        freeJson(key_value_pair->value);
        free(key_value_pair->key);
        free(key_value_pair);
        key_value_pair = next_pair;
    }

    free(json_object);
}

void freeJsonArray(JsonArray *json_array) {
    for (size_t i = 0; i < json_array->array_size; i++) {
        freeJson(json_array->array[i]);
    }

    free(json_array->array);
    free(json_array);
}


int64_t * getInt(JsonObject *json_obj, const char *key) {
    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_INT) return NULL;
    return &(json_value->value->value.val_int);
}

double * getDouble(JsonObject *json_obj, const char *key) {
    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_DOUBLE) return NULL;
    return &(json_value->value->value.val_double);
}

bool * getBool(JsonObject *json_obj, const char *key) {
    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_BOOL) return NULL;
    return &(json_value->value->value.val_bool);
}

char * getString(JsonObject *json_obj, const char *key) {
    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_STRING) return NULL;
    return json_value->value->value.val_string;
}

JsonObject * getObject(JsonObject *json_obj, const char *key) {
    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_OBJECT) return NULL;
    return json_value->value->value.object_ptr;
}

JsonArray * getArray(JsonObject *json_obj, const char *key) {
    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_ARRAY) return NULL;
    return json_value->value->value.array_ptr;
}

char * jsonStringify(Json *json) {
    size_t string_len = 1;

    char *json_string = (char *) calloc(string_len, sizeof(char));
    if (json_string == NULL) {
        perror("malloc");
        fprintf(stderr, "Erro allocating Json string\n");
        return NULL;
    }

    
    return jsonfyRecursive(json, json_string, &string_len);
}

char * jsonfyRecursive(Json *json, char *json_string, size_t *string_len) {
    switch (json->type) {
    case JSON_NULL:
        return jsonfyNull(json, json_string, string_len);
    case JSON_INT:
        return jsonfyInt(json, json_string, string_len);
    case JSON_DOUBLE:
        return jsonfyDouble(json, json_string, string_len);
    case JSON_BOOL:
        return jsonfyBool(json, json_string, string_len);
    case JSON_STRING:
        return jsonfyString(json, json_string, string_len);
    case JSON_OBJECT:
        return jsonfyObject(json, json_string, string_len);
    case JSON_ARRAY:
        return jsonfyArray(json, json_string, string_len);
    default:
        fprintf(stderr, "Invalid JsonType\n");
        break;
    }
}

char * jsonfyNull(Json *json, char *json_string, size_t *string_len) {
    size_t null_size = sizeof("null") - 1;

    *string_len += null_size;
    
    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        perror("realloc");
        fprintf(stderr, "Erro allocating Json string\n");
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, "null");

    return json_string;
}

char * jsonfyInt(Json *json, char *json_string, size_t *string_len) {
    char num[20];
    int num_len = snprintf(num, sizeof(num), "%" PRId64, json->value.val_int);

    if (num_len < 0) {
        perror("snprintf");
        fprintf(stderr, "Erro ao formatar número\n");
        free(json_string);
        return NULL;
    }
    
    *string_len += (size_t) num_len;

    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        perror("realloc");
        fprintf(stderr, "Erro allocating Json string\n");
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, num);

    return json_string;
}

char * jsonfyDouble(Json *json, char *json_string, size_t *string_len) {
    char num[350];
    int num_len = snprintf(num, sizeof(num), "%f", json->value.val_double);

    if (num_len < 0) {
        perror("snprintf");
        fprintf(stderr, "Erro ao formatar número\n");
        return NULL;
    }
    
    *string_len += (size_t) num_len;

    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        perror("realloc");
        fprintf(stderr, "Erro allocating Json string\n");
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, num);

    return json_string;
}

char * jsonfyBool(Json *json, char *json_string, size_t *string_len) {
    const char *bool_string = json->value.val_bool ? "true" : "false";
    size_t bool_size = (json->value.val_bool ? sizeof("true") : sizeof("false")) - 1;

    *string_len += bool_size;
    
    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        perror("realloc");
        fprintf(stderr, "Erro allocating Json string\n");
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, bool_string);

    return json_string;
}

char * jsonfyString(Json *json, char *json_string, size_t *string_len) {
    size_t val_string_len = strlen(json->value.val_string) + 2;

    *string_len += val_string_len;
    
    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        perror("realloc");
        fprintf(stderr, "Erro allocating Json string\n");
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, "\"");
    strcat(json_string, json->value.val_string);
    strcat(json_string, "\"");

    return json_string;
}

char * jsonfyObject(Json *json, char *json_string, size_t *string_len) {
    *string_len += 2;
    
    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        perror("realloc");
        fprintf(stderr, "Erro allocating Json string\n");
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, "{");

    for (JsonKeyValuePair *key_value_pair = json->value.object_ptr->first; key_value_pair != NULL; key_value_pair = key_value_pair->next) {
        size_t key_len = strlen(key_value_pair->key);

        *string_len += key_len + 3;
        new_json_string = (char *) realloc(json_string, *string_len);

        if (new_json_string == NULL) {
            perror("realloc");
            fprintf(stderr, "Erro allocating Json string\n");
            return NULL;
        }

        json_string = new_json_string;

        strcat(json_string, "\"");
        strcat(json_string, key_value_pair->key);
        strcat(json_string, "\":");

        json_string = jsonfyRecursive(key_value_pair->value, json_string, string_len);
        if (json_string == NULL) {
            return NULL;
        }

        if (key_value_pair->next != NULL) {
            *string_len += 1;

            new_json_string = (char *) realloc(json_string, *string_len);
            if (new_json_string == NULL) {
                perror("realloc");
                fprintf(stderr, "Erro allocating Json string\n");
                return NULL;
            }

            json_string = new_json_string;

            strcat(json_string, ",");
        }

    }
    strcat(json_string, "}");

    return json_string;
}

char * jsonfyArray(Json *json, char *json_string, size_t *string_len) {
    *string_len += 2;

    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        perror("realloc");
        fprintf(stderr, "Erro allocating Json string\n");
        free(json_string);
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, "[");

    size_t num_valid_elements = 0;
    for (size_t i = 0; i < json->value.array_ptr->array_size; i++) {
        if (json->value.array_ptr->array[i] != NULL) num_valid_elements++;
    }

    for (size_t i = 0; i < json->value.array_ptr->array_size; i++) {
        if (json->value.array_ptr->array[i] == NULL) continue;
        num_valid_elements--;

        json_string = jsonfyRecursive(json->value.array_ptr->array[i], json_string, string_len);
        if (json_string == NULL) {
            return NULL;
        }

        if (num_valid_elements) {
            *string_len += 1;

            new_json_string = (char *) realloc(json_string, *string_len);
            if (new_json_string == NULL) {
                perror("realloc");
                fprintf(stderr, "Erro allocating Json string\n");
                free(json_string);
                return NULL;
            }

            json_string = new_json_string;

            strcat(json_string, ",");
        }
    }

    strcat(json_string, "]");

    return json_string;
}
