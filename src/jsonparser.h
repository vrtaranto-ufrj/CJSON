#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#include "jsonv2aux.h"

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
Json *parseJson(char **input);

Json * parseNull(Token *token);
Json * parseInt(Token *token);
Json * parseDouble(Token *token);
Json * parseBool(Token *token);
Json * parseString(Token *token);
Json * parseObject(char **input);
Json * parseArray(char **input);