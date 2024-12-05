# CJSON
Biblioteca em C implementando JSON

## Como usar a biblioteca
Para usar a biblioteca, basta incluir o arquivo `json.h` no seu código e compilar o arquivo `json.c` junto com o seu código.

## Funções disponíveis

### Funções de criação
```c
Json * createJson(JsonTypes type, void * value);
JsonObject * createJsonObject();
JsonArray * createJsonArray(size_t array_size);
```

onde JsonTypes é um enum com os seguintes valores:
```c
enum JsonTypesEnum {
    JSON_NULL,
    JSON_INT,
    JSON_DOUBLE,
    JSON_BOOL,
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY
};
```

### Funções para parse e stringify
```c
Json *jsonParse(char *json_string);
char * jsonStringify(Json *json);
```


### Funções de manipulação de objetos JSON
```c
void setKeyValuePair(JsonObject *json_obj, const char *key, Json *value);

int64_t * getInt(JsonObject *json_obj, const char *key);
double * getDouble(JsonObject *json_obj, const char *key);
bool * getBool(JsonObject *json_obj, const char *key);
char * getString(JsonObject *json_obj, const char *key);
JsonObject * getObject(JsonObject *json_obj, const char *key);
JsonArray * getArray(JsonObject *json_obj, const char *key);
```

### Funções de manipulação de arrays JSON
```c
void changeJsonArraySize(JsonArray *json_array, size_t array_size);
int setArrayValue(JsonArray *json_array, size_t index, Json *value);
Json * getArrayValue(JsonArray *json_array, size_t index);
```

### Funções de manipulação de JSON
```c
int setJsonValueType(Json *json, JsonTypes type, void * value);

int setNull(Json *json);
int setInt(Json *json, int64_t val_int);
int setDouble(Json *json, double val_double);
int setBool(Json *json, bool val_bool);
int setString(Json *json, char * val_string);
int setObject(Json *json, JsonObject *obj_ptr);
int setArray(Json *json, JsonArray *array_ptr);
```

### Funções para liberar memória
```c
void freeJson(Json *json);
void freeJsonObject(JsonObject *json_object);
void freeJsonArray(JsonArray *json_array);
```

## Exemplo de uso
O código main.c em /src contém um exemplo de uso da biblioteca.

## Códigos de erro
Funções que retornam ponteiros quando retornam NULL, pode ser que setem o código do errno caso tenho acontecido alguma falha.
Funções que retornam int quando retornam algo diferente de 0, o código do errno será setado.

No .h, funções que retornam erros, ou seja, seta valores de errno, possuem um comentário dizendo quais os possiveis erros que podem ocorrem.

### Códigos de erros
```c
enum JsonErrorsEnum {
    JSON_ERR_ALLOC = 1,
    JSON_ERR_NULL,
    JSON_ERR_INV_TYPE,
    JSON_ERR_OUT_BONDS,
    JSON_ERR_INV_NUM,
    JSON_ERR_INV_JSON_STR
};
```
