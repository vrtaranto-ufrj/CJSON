#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <regex.h>


typedef struct JsonKeyValuePairStruct JsonKeyValuePair;
typedef struct JsonValueStruct JsonValue;
typedef struct JsonObjectStruct JsonObject;
typedef struct JsonArrayStruct JsonArray;

typedef enum JsonTypesEnum JsonTypes;

typedef union JsonUnionUnion JsonUnion;

typedef JsonValue Json;

Json * createJson(JsonTypes type, void * value);
JsonObject * createJsonObject();
JsonArray * createJsonArray(size_t array_size);
void changeJsonArraySize(JsonArray *json_array, size_t array_size);

void setJsonValueType(Json *json, JsonTypes type, void * value);

void freeJson(Json *json);
void freeJsonObject(JsonObject *json_object);
void freeJsonArray(JsonArray *json_array);

int64_t * getInt(JsonObject *json_obj, const char *key);
double * getDouble(JsonObject *json_obj, const char *key);
bool * getBool(JsonObject *json_obj, const char *key);
char * getString(JsonObject *json_obj, const char *key);
JsonObject * getObject(JsonObject *json_obj, const char *key);
JsonArray * getArray(JsonObject *json_obj, const char *key);

char * jsonStringify(Json *json);


enum JsonTypesEnum {
    JSON_NULL,
    JSON_INT,
    JSON_DOUBLE,
    JSON_BOOL,
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY
};

union JsonUnionUnion {
    int64_t val_int;
    double val_double;
    bool val_bool;
    char *val_string;
    JsonObject *object_ptr;
    JsonArray *array_ptr;
};

struct JsonValueStruct {
    JsonUnion value;
    JsonTypes type;
};

struct JsonKeyValuePairStruct {
    char *key;
    JsonValue *value;
    JsonKeyValuePair *next;
    JsonKeyValuePair *previous;
};

struct JsonObjectStruct {
    JsonKeyValuePair *first;
    JsonKeyValuePair *last;
};

struct JsonArrayStruct {
    JsonValue **array;
    size_t array_size;
};

