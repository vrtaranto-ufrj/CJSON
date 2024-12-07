#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "json.h"

#define NUM_BASE 10

char * jsonfyRecursive(Json *json, char *json_string, size_t *string_len);
char * jsonfyNull(Json *json, char *json_string, size_t *string_len);
char * jsonfyInt(Json *json, char *json_string, size_t *string_len);
char * jsonfyDouble(Json *json, char *json_string, size_t *string_len);
char * jsonfyBool(Json *json, char *json_string, size_t *string_len);
char * jsonfyString(Json *json, char *json_string, size_t *string_len);
char * jsonfyObject(Json *json, char *json_string, size_t *string_len);
char * jsonfyArray(Json *json, char *json_string, size_t *string_len);

int addKeyValuePair(JsonObject *json_obj, const char *key, Json *value);

JsonKeyValuePair * findKeyValuePair(JsonObject *json_obj, const char *key);  


Json * createJson(JsonTypes type, void * value) {
    if (value == NULL && type != JSON_NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }
        

    Json * new_json = (Json *) calloc(1, sizeof(Json));
    if (new_json == NULL) {
        errno = JSON_ERR_ALLOC;
        return NULL;
    }

    setJsonValueType(new_json, type, value);

    return new_json;
}

JsonObject * createJsonObject() {
    JsonObject * new_json_obj = (JsonObject *) calloc(1, sizeof(JsonObject));
    if (new_json_obj == NULL) {
        errno = JSON_ERR_ALLOC;
        return NULL;
    }

    return new_json_obj;
}

JsonArray * createJsonArray(size_t array_size) {
    JsonArray * new_json_array = (JsonArray *) malloc(sizeof(JsonArray));
    if (new_json_array == NULL) {
        errno = JSON_ERR_ALLOC;
        return NULL;
    }

    new_json_array->array = (JsonValue **) calloc(array_size, sizeof(JsonValue*));
    if (new_json_array->array == NULL) {
        errno = JSON_ERR_ALLOC;
        return NULL;
    } 

    new_json_array->array_size = array_size;

    return new_json_array;
}

void changeJsonArraySize(JsonArray *json_array, size_t array_size) {
    json_array->array = (JsonValue **) realloc(json_array->array, sizeof(JsonValue*) * array_size);
    if (json_array->array == NULL) {
        perror("realloc");
        fprintf(stderr, "Failed to realloc JsonArray\n");
        return;
    }
    if (array_size > json_array->array_size) {
        memset(&(json_array->array[json_array->array_size]), 0, (array_size - json_array->array_size) * sizeof(JsonValue*));
    }

    json_array->array_size = array_size;
}

int setJsonValueType(Json *json, JsonTypes type, void * value) {
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
        if (setString(json, (char*) value) == -1) return -1;
        break;
    case JSON_OBJECT:
        setObject(json, (JsonObject*) value);
        break;
    case JSON_ARRAY:
        setArray(json, (JsonArray*) value);
        break;
    default:
        errno = JSON_ERR_INV_TYPE;
        return -1;
    }

    return 0;
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
    if (json_obj == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }

    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_INT) return NULL;
    return &(json_value->value->value.val_int);
}

double * getDouble(JsonObject *json_obj, const char *key) {
    if (json_obj == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }

    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_DOUBLE) return NULL;
    return &(json_value->value->value.val_double);
}

bool * getBool(JsonObject *json_obj, const char *key) {
    if (json_obj == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }

    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_BOOL) return NULL;
    return &(json_value->value->value.val_bool);
}

char * getString(JsonObject *json_obj, const char *key) {
    if (json_obj == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }

    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_STRING) return NULL;
    return json_value->value->value.val_string;
}

JsonObject * getObject(JsonObject *json_obj, const char *key) {
    if (json_obj == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }

    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_OBJECT) return NULL;
    return json_value->value->value.object_ptr;
}

JsonArray * getArray(JsonObject *json_obj, const char *key) {
    if (json_obj == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }

    JsonKeyValuePair *json_value = findKeyValuePair(json_obj, key);
    if (json_value == NULL || json_value->value->type != JSON_ARRAY) return NULL;

    return json_value->value->value.array_ptr;
}

Json * getArrayValue(JsonArray *json_array, size_t index) {
    if (json_array == NULL || json_array->array == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }
    if (index >= json_array->array_size) {
        errno = JSON_ERR_OUT_BONDS;
        return NULL;
    }

    return json_array->array[index];
}


char * jsonStringify(Json *json) {
    if (json == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }

    size_t string_len = 1;

    char *json_string = (char *) calloc(string_len, sizeof(char));
    if (json_string == NULL) {
        errno = JSON_ERR_ALLOC;
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
        errno = JSON_ERR_INV_TYPE;
    }

    return NULL;
}

char * jsonfyNull(Json *json, char *json_string, size_t *string_len) {
    size_t null_size = sizeof("null") - 1;

    *string_len += null_size;
    
    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        free(json_string);
        errno = JSON_ERR_ALLOC;
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
        free(json_string);
        errno = JSON_ERR_INV_NUM;
        return NULL;
    }
    
    *string_len += (size_t) num_len;

    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        free(json_string);
        errno = JSON_ERR_ALLOC;
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
        free(json_string);
        errno = JSON_ERR_INV_NUM;
        return NULL;
    }
    
    *string_len += (size_t) num_len;

    char *new_json_string = (char *) realloc(json_string, *string_len);
    if (new_json_string == NULL) {
        free(json_string);
        errno = JSON_ERR_ALLOC;
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
        errno = JSON_ERR_ALLOC;
        free(json_string);
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
        free(json_string);
        errno = JSON_ERR_ALLOC;
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
        free(json_string);
        errno = JSON_ERR_ALLOC;
        return NULL;
    }

    json_string = new_json_string;

    strcat(json_string, "{");

    for (JsonKeyValuePair *key_value_pair = json->value.object_ptr->first; key_value_pair != NULL; key_value_pair = key_value_pair->next) {
        size_t key_len = strlen(key_value_pair->key);

        *string_len += key_len + 3;

        new_json_string = (char *) realloc(json_string, *string_len);
        if (new_json_string == NULL) {
            free(json_string);
            errno = JSON_ERR_ALLOC;
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
                free(json_string);
                errno = JSON_ERR_ALLOC;
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
        free(json_string);
        errno = JSON_ERR_ALLOC;
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
                free(json_string);
                errno = JSON_ERR_ALLOC;
                return NULL;
            }

            json_string = new_json_string;

            strcat(json_string, ",");
        }
    }

    strcat(json_string, "]");

    return json_string;
}

JsonKeyValuePair * findKeyValuePair(JsonObject *json_obj, const char *key) {    
    for (JsonKeyValuePair *key_value_pair = json_obj->first; key_value_pair != NULL; key_value_pair = key_value_pair->next) {
        if (strcmp(key_value_pair->key, key) == 0) {
            return key_value_pair;
        }
    }

    return NULL;
}

int setKeyValuePair(JsonObject *json_obj, const char *key, Json *value) {
    if (value == NULL) {
        errno = JSON_ERR_NULL;
        return -1;
    }


    JsonKeyValuePair *old_json_key_value_pair = findKeyValuePair(json_obj, key);
    if (old_json_key_value_pair == NULL) {
        addKeyValuePair(json_obj, key, value);
        return 0;
    }

    freeJson(value);

    old_json_key_value_pair->value = value;

    return 0;
}

int setArrayValue(JsonArray *json_array, size_t index, Json *value) {
    if (json_array == NULL) {
        errno = JSON_ERR_NULL;
        return -1;
    }
    if (index >= json_array->array_size ) {
        errno = JSON_ERR_OUT_BONDS;
        return -1;
    }

    json_array->array[index] = value;

    return 0;
}

int addKeyValuePair(JsonObject *json_obj, const char *key, Json *value) {
    JsonKeyValuePair * key_value_pair = (JsonKeyValuePair *) malloc(sizeof(JsonKeyValuePair));
    if (key_value_pair == NULL) {
        errno = JSON_ERR_ALLOC;
        return -1;
    }

    size_t key_size = strlen(key) + 1;
    key_value_pair->key = (char *) malloc(sizeof(char) * key_size);
    if (key_value_pair->key == NULL) {
        errno = JSON_ERR_ALLOC;
        return -1;
    }
    
    strncpy(key_value_pair->key, key, key_size);
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

    return 0;
}

int setNull(Json *json) {
    memset(json, 0, sizeof(JsonValue));
    return 0;
}

int setInt(Json *json, int64_t val_int) {
    json->type = JSON_INT;
    json->value.val_int = val_int;
    return 0;
}

int setDouble(Json *json, double val_double) {
    json->type = JSON_DOUBLE;
    json->value.val_double = val_double;
    return 0;
}

int setBool(Json *json, bool val_bool) {
    json->type = JSON_BOOL;
    json->value.val_bool = val_bool;
    return 0;
}

int setString(Json *json, char * val_string) {
    size_t string_size = strlen(val_string) + 1;

    json->value.val_string = (char *) malloc(string_size);
    if (json->value.val_string == NULL) {
        errno = JSON_ERR_ALLOC;
        return -1;
    }

    strncpy(json->value.val_string, val_string, string_size);
    json->type = JSON_STRING;
    return 0;
}

int setObject(Json *json, JsonObject *obj_ptr) {
    json->type = JSON_OBJECT;
    json->value.object_ptr = obj_ptr;
    return 0;
}

int setArray(Json *json, JsonArray *array_ptr) {
    json->type = JSON_ARRAY;
    json->value.array_ptr = array_ptr;
    return 0;
}

// Json Parser

Json *parseJson(char **input);
Json * parseNull(Token *token);
Json * parseInt(Token *token);
Json * parseDouble(Token *token);
Json * parseBool(Token *token);
Json * parseString(Token *token);
Json * parseObject(char **input);
Json * parseArray(char **input);

Token *nextToken(char **input);

Token *nextToken(char **input) {
    while (isspace(**input)) (*input)++; // Ignorar espaços em branco

    char c = **input;

    if (c == '\0') return NULL;

    Token *token = malloc(sizeof(Token));
    if (token == NULL) {
        errno = JSON_ERR_ALLOC;
        return NULL;
    }

    if (c == '{') {
        token->type = TOKEN_LEFT_BRACE;
        token->value = NULL;
        (*input)++;
    } else if (c == '}') {
        token->type = TOKEN_RIGHT_BRACE;
        token->value = NULL;
        (*input)++;
    } else if (c == '[') {
        token->type = TOKEN_LEFT_BRACKET;
        token->value = NULL;
        (*input)++;
    } else if (c == ']') {
        token->type = TOKEN_RIGHT_BRACKET;
        token->value = NULL;
        (*input)++;
    } else if (c == ':') {
        token->type = TOKEN_COLON;
        token->value = NULL;
        (*input)++;
    } else if (c == ',') {
        token->type = TOKEN_COMMA;
        token->value = NULL;
        (*input)++;
    } else if (strncmp(*input, "true", 4) == 0) {
        token->type = TOKEN_TRUE;
        token->value = NULL;
        (*input) += 4;
    } else if (strncmp(*input, "false", 5) == 0) {
        token->type = TOKEN_FALSE;
        token->value = NULL;
        (*input) += 5;
    } else if (strncmp(*input, "null", 4) == 0) {
        token->type = TOKEN_NULL;
        token->value = NULL;
        (*input) += 4;
    } else if (c == '"') {
        (*input)++;
        const char *start = *input;
        while (**input != '"' && **input != '\0') (*input)++;
        if (**input == '"') {
            size_t length = *input - start;
            token->type = TOKEN_STRING;
            token->value = malloc(length + 1);
            if (token->value == NULL) {
                free(token);
                errno = JSON_ERR_ALLOC;
                return NULL;
            }
            strncpy(token->value, start, length);
            token->value[length] = '\0';
            (*input)++;
        } else {
            free(token);
            errno = JSON_ERR_INV_JSON_STR;
            return NULL;
        }
    } else if (isdigit(c) || c == '-') {
        const char *start = *input;
        bool isDouble = false;
        while (isdigit(**input) || **input == '.' || **input == 'e' || **input == 'E' || **input == '+' || **input == '-') {
            if (!isDouble && (**input == 'e' || **input == 'E' || **input == '.')) {
                isDouble = true;
            }
            (*input)++;
        }
        size_t length = *input - start;
        token->value = malloc(length + 1);
        if (token == NULL) {
            free(token);
            errno = JSON_ERR_ALLOC;
            return NULL;
        }
        strncpy(token->value, start, length);
        token->value[length] = '\0';
        token->type = isDouble ? TOKEN_DOUBLE : TOKEN_INT;
    } else {
        free(token->value);
        errno = JSON_ERR_INV_JSON_STR;
        return NULL;
    }

    return token;
}

Json *parseJson(char **input) {
    Token *token = nextToken(input);
    Json *json;
    if (token == NULL) return NULL;

    switch (token->type) {
    case TOKEN_NULL:
        json = parseNull(token);
        break;
    case TOKEN_INT:
        json = parseInt(token);
        break;
    case TOKEN_DOUBLE:
        json = parseDouble(token);
        break;
    case TOKEN_TRUE:
    case TOKEN_FALSE:
        json = parseBool(token);
        break;
    case TOKEN_STRING:
        json = parseString(token);
        break;
    case TOKEN_LEFT_BRACE:
        json = parseObject(input);
        break;
    case TOKEN_LEFT_BRACKET:
        json = parseArray(input);
        break;
    default:
        errno = JSON_ERR_INV_TYPE;
        json = NULL;
    }

    free(token);
    return json;
}

Json *jsonParse(char *json_string) {
    if (json_string == NULL) {
        errno = JSON_ERR_NULL;
        return NULL;
    }
    char *string;
    size_t json_len = strlen(json_string);

    string = (char *) malloc(json_len + 1);
    if (string == NULL) {
        errno = JSON_ERR_ALLOC;
        return NULL;
    }

    strncpy(string, json_string, json_len);

    char *temp_input = string;
    Json *json = parseJson(&temp_input);
    free(string);

    return json;
}

Json * parseNull(Token *token) {
    return createJson(JSON_NULL, NULL);
}

Json * parseInt(Token *token) {
    char *endptr;
    int64_t val_int = strtoll(token->value, &endptr, NUM_BASE);
    if (endptr != NULL && *endptr != '\0') {
        free(token->value);
        errno = JSON_ERR_INV_NUM;
        return NULL;
    }
    free(token->value);
    return createJson(JSON_INT, &val_int);
}

Json * parseDouble(Token *token) {
    char *endptr;
    double val_double = strtod(token->value, &endptr);
    if (endptr != NULL && *endptr != '\0') {
        free(token->value);
        errno = JSON_ERR_INV_NUM;
        return NULL;
    }
    free(token->value);
    return createJson(JSON_DOUBLE, &val_double);
}

Json * parseBool(Token *token) {
    bool val_bool = token->type == TOKEN_TRUE ? true : false;
    return createJson(JSON_BOOL, &val_bool);
}

Json * parseString(Token *token) {
    Json * json = createJson(JSON_STRING, token->value);
    free(token->value);
    return json;
}

Json * parseObject(char **input) {
    JsonObject *obj = createJsonObject();
    Json *json_obj = createJson(JSON_OBJECT, obj);

    Json *json_value;
    char *key;

    while (1) {
        Token *token = nextToken(input);
        if (token == NULL) return NULL;

        if (token->type != TOKEN_STRING) return NULL;

        key = token->value;

        free(token);
        token = nextToken(input);
        if (token == NULL) return NULL;

        if (token->type != TOKEN_COLON) return NULL;

        free(token);
        token = nextToken(input);
        if (token == NULL) return NULL;

        switch (token->type) {
        case TOKEN_NULL:
            json_value = parseNull(token);
            break;
        case TOKEN_INT:
            json_value = parseInt(token);
            break;
        case TOKEN_DOUBLE:
            json_value = parseDouble(token);
            break;
        case TOKEN_TRUE:
        case TOKEN_FALSE:
            json_value = parseBool(token);
            break;
        case TOKEN_STRING:
            json_value = parseString(token);
            break;
        case TOKEN_LEFT_BRACE:
            json_value = parseObject(input);
            break;
        case TOKEN_LEFT_BRACKET:
            json_value = parseArray(input);
            break;
        default:
            errno = JSON_ERR_INV_TYPE;
            return NULL;
        }

        setKeyValuePair(obj, key, json_value);

        free(key);
        free(token);
        token = nextToken(input);
        if (token == NULL) return NULL;
        

        if (token->type == TOKEN_RIGHT_BRACE) {
            free(token);
            return json_obj;
        } else if (token->type != TOKEN_COMMA || token->type == TOKEN_EOF) {
            free(token);
            errno = JSON_ERR_INV_JSON_STR;
            return NULL;
        }
        free(token);
        
    }

}

Json * parseArray(char **input) {
    JsonArray *array = createJsonArray(16);
    Json *json_array = createJson(JSON_ARRAY, array);
    Json *json_value;
    size_t index = 0;

    while (1) {
        Token *token = nextToken(input);
        if (token == NULL) return NULL;

        switch (token->type) {
        case TOKEN_NULL:
            json_value = parseNull(token);
            break;
        case TOKEN_INT:
            json_value = parseInt(token);
            break;
        case TOKEN_DOUBLE:
            json_value = parseDouble(token);
            break;
        case TOKEN_TRUE:
        case TOKEN_FALSE:
            json_value = parseBool(token);
            break;
        case TOKEN_STRING:
            json_value = parseString(token);
            break;
        case TOKEN_LEFT_BRACE:
            json_value = parseObject(input);
            break;
        case TOKEN_LEFT_BRACKET:
            json_value = parseArray(input);
            break;
        default:
            fprintf(stderr, "Unknown TOKEN\n");
            return NULL;
        }
        
        if (index >= array->array_size) changeJsonArraySize(array, array->array_size * 2);

        setArrayValue(array, index++, json_value);

        free(token);
        token = nextToken(input);
        if (token == NULL) return NULL;

        if (token->type == TOKEN_RIGHT_BRACKET) {
            free(token);
            changeJsonArraySize(array, array->array_size);
            return json_array;
        } else if (token->type != TOKEN_COMMA || token->type == TOKEN_EOF) {
            free(token);
            return NULL;
        }
        free(token);
    }
}
