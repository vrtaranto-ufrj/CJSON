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
void setArrayValue(JsonArray *json_array, size_t index, Json *value);
Json * getArrayValue(JsonArray *json_array, size_t index);
```

### Funções de manipulação de JSON
```c
void setJsonValueType(Json *json, JsonTypes type, void * value);

void setNull(Json *json);
void setInt(Json *json, int64_t val_int);
void setDouble(Json *json, double val_double);
void setBool(Json *json, bool val_bool);
void setString(Json *json, char * val_string);
void setObject(Json *json, JsonObject *obj_ptr);
void setArray(Json *json, JsonArray *array_ptr);
```

### Funções para liberar memória
```c
void freeJson(Json *json);
void freeJsonObject(JsonObject *json_object);
void freeJsonArray(JsonArray *json_array);
```

## Exemplo de uso
O código main.c em /src contém um exemplo de uso da biblioteca.