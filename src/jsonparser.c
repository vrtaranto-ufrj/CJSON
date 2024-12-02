#include "jsonparser.h"

Token *nextToken(char **input) {
    while (isspace(**input)) (*input)++; // Ignorar espaços em branco

    char c = **input;

    if (c == '\0') return NULL;

    Token *token = malloc(sizeof(Token));

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
            strncpy(token->value, start, length);
            token->value[length] = '\0';
            (*input)++;
        } else {
            token->type = TOKEN_ERROR;
            token->value = NULL;
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
        strncpy(token->value, start, length);
        token->value[length] = '\0';
        token->type = isDouble ? TOKEN_DOUBLE : TOKEN_INT;
    } else {
        token->type = TOKEN_ERROR;
        token->value = NULL;
        (*input)++;
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
        fprintf(stderr, "Unknown TOKEN\n");
        return NULL;
    }

    free(token);
    return json;
}

Json *jsonParse(char *json_string) {
    char *string;
    size_t json_len = strlen(json_string);

    string = (char *) malloc(json_len + 1);

    strncpy(string, json_string, json_len);

    char *temp_input = string;
    Json *json = parseJson(&temp_input);
    free(string);

    if (json == NULL) {
        fprintf(stderr, "Unable to parse json. Not a valid format!\n");
    }

    return json;
}


Json * parseNull(Token *token) {
    return createJson(JSON_NULL, NULL);
}

Json * parseInt(Token *token) {
    int64_t val_int = strtoll(token->value, NULL, 10);
    free(token->value);
    return createJson(JSON_INT, &val_int);
}

Json * parseDouble(Token *token) {
    double val_double = strtof(token->value, NULL);
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

        if (token->type != TOKEN_COLON) return NULL;

        free(token);
        token = nextToken(input);

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

        setKeyValuePair(obj, key, json_value);

        free(key);
        free(token);
        token = nextToken(input);
        

        if (token->type == TOKEN_RIGHT_BRACE) {
            free(token);
            return json_obj;
        } else if (token->type != TOKEN_COMMA || token->type == TOKEN_EOF) {
            free(token);
            return NULL;
        }
        free(token);
        
    }

}

Json * parseArray(char **input) {
    JsonArray *array = createJsonArray(16);
    Json *json_array = createJson(JSON_ARRAY, array);
    Json *json_value;
    char *key;
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