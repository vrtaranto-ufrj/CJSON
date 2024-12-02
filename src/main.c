#include <stdio.h>

#include "jsonv2.h"
#include "jsonv2aux.h"


int main(void) {
    Json *json = createJson(JSON_OBJECT, createJsonObject());
    

    setKeyValuePair(json->value.object_ptr, "oi", createJson(JSON_STRING, "Ola"));
    setKeyValuePair(json->value.object_ptr, "ola", createJson(JSON_ARRAY, createJsonArray(2)));
    

    int64_t a = 13;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 0, createJson(JSON_INT, &a));

    bool b = true;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 1, createJson(JSON_BOOL, &b));

    const char *i = getString(json->value.object_ptr, "oi");

    char *json_string = jsonfy(json);

    printf("%s\n", json_string);

    free(json_string);
    freeJson(json);

    return 0;
}
