#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>


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

Json * getArrayValue(JsonArray *json_array, size_t index);

char * jsonStringify(Json *json);

void setKeyValuePair(JsonObject *json_obj, const char *key, Json *value);
void setArrayValue(JsonArray *json_array, size_t index, Json *value);

void setNull(Json *json);
void setInt(Json *json, int64_t val_int);
void setDouble(Json *json, double val_double);
void setBool(Json *json, bool val_bool);
void setString(Json *json, char * val_string);
void setObject(Json *json, JsonObject *obj_ptr);
void setArray(Json *json, JsonArray *array_ptr);


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


// Json Parser

typedef struct TokenStruct Token;
typedef enum TokenTypeEnum TokenType;

enum TokenTypeEnum {
    TOKEN_STRING,
    TOKEN_INT,
    TOKEN_DOUBLE,
    TOKEN_LEFT_BRACE,  // {
    TOKEN_RIGHT_BRACE, // }
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_COLON,        // :
    TOKEN_COMMA,        // ,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_EOF,
    TOKEN_ERROR
} ;

struct TokenStruct {
    TokenType type;
    char *value; // Usado apenas para strings e números
};

Json *jsonParse(char *json_string);
