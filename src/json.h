#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>


typedef struct JsonKeyValuePairStruct JsonKeyValuePair;
typedef struct JsonValueStruct JsonValue;
typedef struct JsonObjectStruct JsonObject;
typedef struct JsonArrayStruct JsonArray;

typedef enum JsonTypesEnum JsonTypes;
typedef enum JsonErrorsEnum JsonErrors;

typedef union JsonUnionUnion JsonUnion;

typedef JsonValue Json;

// @errno JSON_ERR_NULL, JSON_ERR_ALLOC, JSON_ERR_INV_TYPE
Json * createJson(JsonTypes type, void * value);
// @errno JSON_ERR_ALLOC
JsonObject * createJsonObject();
// @errno JSON_ERR_ALLOC
JsonArray * createJsonArray(size_t array_size);
void changeJsonArraySize(JsonArray *json_array, size_t array_size);

int setJsonValueType(Json *json, JsonTypes type, void * value);

void freeJson(Json *json);
void freeJsonObject(JsonObject *json_object);
void freeJsonArray(JsonArray *json_array);

// @errno JSON_ERR_NULL
int64_t * getInt(JsonObject *json_obj, const char *key);
// @errno JSON_ERR_NULL
double * getDouble(JsonObject *json_obj, const char *key);
// @errno JSON_ERR_NULL
bool * getBool(JsonObject *json_obj, const char *key);
// @errno JSON_ERR_NULL
char * getString(JsonObject *json_obj, const char *key);
// @errno JSON_ERR_NULL
JsonObject * getObject(JsonObject *json_obj, const char *key);
// @errno JSON_ERR_NULL
JsonArray * getArray(JsonObject *json_obj, const char *key);
// @errno JSON_ERR_NULL JSON_ERR_OUT_BONDS
Json * getArrayValue(JsonArray *json_array, size_t index);

// @errno JSON_ERR_NULL, JSON_ERR_ALLOC, JSON_ERR_INV_TYPE, JSON_ERR_INV_NUM
char * jsonStringify(Json *json);

// @errno JSON_ERR_NULL, JSON_ERR_ALLOC
int setKeyValuePair(JsonObject *json_obj, const char *key, Json *value);
// @errno JSON_ERR_NULL, JSON_ERR_OUT_BONDS
int setArrayValue(JsonArray *json_array, size_t index, Json *value);

int setNull(Json *json);
int setInt(Json *json, int64_t val_int);
int setDouble(Json *json, double val_double);
int setBool(Json *json, bool val_bool);
int setString(Json *json, char * val_string);
int setObject(Json *json, JsonObject *obj_ptr);
int setArray(Json *json, JsonArray *array_ptr);


enum JsonTypesEnum {
    JSON_NULL,
    JSON_INT,
    JSON_DOUBLE,
    JSON_BOOL,
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY
};

enum JsonErrorsEnum {
    JSON_ERR_ALLOC = 1,
    JSON_ERR_NULL,
    JSON_ERR_INV_TYPE,
    JSON_ERR_OUT_BONDS,
    JSON_ERR_INV_NUM,
    JSON_ERR_INV_JSON_STR
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
    TOKEN_EOF
} ;

struct TokenStruct {
    TokenType type;
    char *value; // Usado apenas para strings e números
};

// @errno JSON_ERR_ALLOC, JSON_ERR_NULL, JSON_ERR_INV_JSON_STR
Json *jsonParse(char *json_string);
