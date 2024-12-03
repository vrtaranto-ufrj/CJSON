#include <stdio.h>

#include "jsonv2.h"


int main(void) {
    Json *json = createJson(JSON_OBJECT, createJsonObject());
    

    setKeyValuePair(json->value.object_ptr, "ola", createJson(JSON_ARRAY, createJsonArray(2)));
    setKeyValuePair(json->value.object_ptr, "oi", createJson(JSON_STRING, "Ola"));
    

    int64_t a = 13;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 0, createJson(JSON_INT, &a));

    bool b = true;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 1, createJson(JSON_BOOL, &b));

    const char *i = getString(json->value.object_ptr, "oi");

    char *json_string = jsonStringify(json);

    printf("%s\n", json_string);
    
    Json *teste = jsonParse(
      "{\n"
      "\"ola\": [\n"
        "13,\n"
        "true\n"
      "],\n"
      "\"oi\": \"Ola\"\n"
    "}\n"
    );

    free(json_string);

    json_string = jsonStringify(teste);

    printf("%s\n", json_string);

    free(json_string);
    freeJson(json);
    freeJson(teste);

    return 0;
}
