#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "json.h"

bool testcreateJsonObjectFunction() {
    JsonObject *json_object = createJsonObject();
    bool worked = true;

    if (json_object == NULL || json_object->first != NULL || json_object->last != NULL) {
        worked = false;
    }
    freeJsonObject(json_object);    
    return worked;
}

bool testcreateJsonArrayFunction() {
    size_t arraySize = 153;
    bool worked = true;

    JsonArray *json_array = createJsonArray(arraySize);
    if (json_array == NULL || json_array->array == NULL || json_array->array_size != arraySize) {
        worked =  false;
    }
    freeJsonArray(json_array);
    return worked;
}

bool testcreateJsonArrayFunctionSizeZero() {
    size_t arraySize = 0;
    bool worked = true;

    JsonArray *json_array = createJsonArray(arraySize);
    if (json_array == NULL || json_array->array == NULL || json_array->array_size != arraySize) {
        worked =  false;
    }
    freeJsonArray(json_array);
    return worked;
}

bool testcreateJsonFunctionObject() {
    bool worked = true;

    Json *json = createJson(JSON_OBJECT, createJsonObject());
    if (json->type != JSON_OBJECT || json->value.object_ptr == NULL) {
        worked =  false;
    }
    freeJson(json);
    return worked;
}

bool testcreateJsonFunctionArray() {
    size_t arraySize = 101;
    bool worked = true;

    Json *json = createJson(JSON_ARRAY, createJsonArray(arraySize));
    if (json->type != JSON_ARRAY || json->value.array_ptr == NULL) {
        worked = false;
    }
    freeJson(json);
    return worked;
}

bool testcreateJsonFunctionInt() {
    int64_t int_val = 5621;
    bool worked = true;

    Json *json = createJson(JSON_INT, &int_val);
    if (json->type != JSON_INT || json->value.val_int != int_val) {
        worked = false;
    }
    freeJson(json);
    return worked;
}

bool testcreateJsonFunctionDouble() {
    double double_val = 3.14159;
    bool worked = true;

    Json *json = createJson(JSON_DOUBLE, &double_val);
    if (json->type != JSON_DOUBLE || json->value.val_double != double_val) {
        worked = false;
    }
    freeJson(json);
    return worked;
}

bool testcreateJsonFunctionBool() {
    bool bool_val = true;
    bool worked = true;

    Json *json = createJson(JSON_BOOL, &bool_val);
    if (json->type != JSON_BOOL || json->value.val_bool != bool_val) {
        worked = false;
    }
    freeJson(json);
    return worked;
}

bool testcreateJsonFunctionCharPtr() {
    char * string_val = "Ola Mundo";
    bool worked = true;

    Json *json = createJson(JSON_STRING, string_val);
    if (json->type != JSON_STRING || strcmp(json->value.val_string, string_val) != 0)  {
        worked = false;
    }
    freeJson(json);
    return worked;
}

bool testcreateJsonFunctionCharArray() {
    char string_val[] = "Ola Mundo";
    bool worked = true;

    Json *json = createJson(JSON_STRING, string_val);
    if (json->type != JSON_STRING || strcmp(json->value.val_string, string_val) != 0) {
        worked = false;
    }
    freeJson(json);
    return worked;
}

bool testcreateJsonFunctionNull() {
    bool worked = true;

    Json *json = createJson(JSON_NULL, NULL);
    int erro = errno;
    if (json->type != JSON_NULL || json->value.val_int != 0) {
        worked = false;
    }

    freeJson(json);
    return worked;
}

bool testChangeJsonArraySizeFuntionIncrease() {
    size_t arraySize = 100;
    size_t position = 95;
    bool worked = true;

    JsonArray *jsonArray = createJsonArray(arraySize);

    double val_double = 3.1415;
    if (setArrayValue(jsonArray, position, createJson(JSON_DOUBLE, &val_double))) {
        freeJsonArray(jsonArray);
        return false;
    }

    arraySize = 300;
    changeJsonArraySize(jsonArray, arraySize);

    if (jsonArray == NULL || jsonArray->array_size != arraySize || getArrayValue(jsonArray, position)->value.val_double != val_double) {
        worked = false;
    }

    freeJsonArray(jsonArray);
    return worked;
}

bool testChangeJsonArraySizeFuntionDecrease() {
    size_t arraySize = 300;
    size_t position = 95;
    bool worked = true;

    JsonArray *jsonArray = createJsonArray(arraySize);

    double val_double = 3.1415;
    if (setArrayValue(jsonArray, position, createJson(JSON_DOUBLE, &val_double))) {
        freeJsonArray(jsonArray);
        return false;
    }

    arraySize = 100;
    changeJsonArraySize(jsonArray, arraySize);

    if (jsonArray == NULL || jsonArray->array_size != arraySize || getArrayValue(jsonArray, position)->value.val_double != val_double) {
        worked = false;
    }

    freeJsonArray(jsonArray);
    return worked;
}

bool testSetArrayValueFunction() {
    Json *json = createJson(JSON_STRING, "Ola mundo");
    JsonArray *jsonArray = createJsonArray(125);
    bool worked = true;

    if (setArrayValue(jsonArray, 39, json)) {
        freeJson(json);
        freeJsonArray(jsonArray);
        return false;
    }

    if (strcmp(json->value.val_string, jsonArray->array[39]->value.val_string) != 0) {
        worked =  false;
    }

    freeJsonArray(jsonArray);

    return worked;
}

bool testSetArrayValueFunctionOutOfBounds() {
    Json *json = createJson(JSON_STRING, "Ola mundo");
    JsonArray *jsonArray = createJsonArray(2);
    bool worked = true;

    if (setArrayValue(jsonArray, 39, json) != -1 && errno == JSON_ERR_OUT_BONDS) {
        worked = false;
    }

    freeJson(json);
    freeJsonArray(jsonArray);

    return worked;
}

bool testGetArrayValueFunction() {
    Json *json = createJson(JSON_STRING, "Ola mundo");
    JsonArray *jsonArray = createJsonArray(125);
    bool worked = true;

    if (setArrayValue(jsonArray, 39, json)) {
        freeJson(json);
        freeJsonArray(jsonArray);
        return false;
    }

    if (strcmp(json->value.val_string, getArrayValue(jsonArray, 39)->value.val_string) != 0) {
        worked = false;
    }

    freeJsonArray(jsonArray);

    return worked;
}

bool testJsonStringifyFunction() {
    Json *json = createJson(JSON_OBJECT, createJsonObject());
    bool worked = true;
    
    setKeyValuePair(json->value.object_ptr, "ola", createJson(JSON_ARRAY, createJsonArray(2)));
    setKeyValuePair(json->value.object_ptr, "oi", createJson(JSON_STRING, "Ola"));
    

    int64_t a = 13;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 0, createJson(JSON_INT, &a));

    bool b = true;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 1, createJson(JSON_BOOL, &b));

    const char *i = getString(json->value.object_ptr, "oi");

    char *json_string = jsonStringify(json);

    char expected[] = "{\"ola\":[13,true],\"oi\":\"Ola\"}";

    if (json_string == NULL || strcmp(json_string, expected) != 0) {
        worked = false;
    }

    freeJson(json);
    free(json_string);

    return worked;
}

bool testJsonParseFunction() {
    Json *teste = jsonParse(
    "{\n"
      "\"ola\": [\n"
        "13,\n"
        "true\n"
      "],\n"
      "\"oi\": \"Ola\"\n"
    "}\n"
    );
    bool worked = true;

    Json *json = createJson(JSON_OBJECT, createJsonObject());
    
    setKeyValuePair(json->value.object_ptr, "ola", createJson(JSON_ARRAY, createJsonArray(2)));
    setKeyValuePair(json->value.object_ptr, "oi", createJson(JSON_STRING, "Ola"));
    

    int64_t a = 13;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 0, createJson(JSON_INT, &a));

    bool b = true;
    setArrayValue(getArray(json->value.object_ptr, "ola"), 1, createJson(JSON_BOOL, &b));

    const char *i = getString(json->value.object_ptr, "oi");

    if (strcmp(getString(json->value.object_ptr, "oi"), getString(teste->value.object_ptr, "oi")) != 0) return false;
    
    if (getArrayValue(getArray(json->value.object_ptr, "ola"), 0)->value.val_int != getArrayValue(getArray(teste->value.object_ptr, "ola"), 0)->value.val_int) worked = false;
    else if (getArrayValue(getArray(json->value.object_ptr, "ola"), 1)->value.val_bool != getArrayValue(getArray(teste->value.object_ptr, "ola"), 1)->value.val_bool) worked = false;

    freeJson(json);
    freeJson(teste);

    return worked;
}

int main(void) {
    bool (*fun_ptr[])() = {
        testcreateJsonObjectFunction,
        testcreateJsonArrayFunction,
        testcreateJsonArrayFunctionSizeZero,
        testcreateJsonFunctionObject,
        testcreateJsonFunctionArray,
        testcreateJsonFunctionInt,
        testcreateJsonFunctionDouble,
        testcreateJsonFunctionBool,
        testcreateJsonFunctionCharPtr,
        testcreateJsonFunctionCharArray,
        testcreateJsonFunctionNull,
        testChangeJsonArraySizeFuntionIncrease,
        testChangeJsonArraySizeFuntionDecrease,
        testSetArrayValueFunction,
        testSetArrayValueFunctionOutOfBounds,
        testGetArrayValueFunction,
        testJsonStringifyFunction,
        testJsonParseFunction,
    };

    char *nameTests[] = {
        "testcreateJsonObjectFunction",
        "testcreateJsonArrayFunction",
        "testcreateJsonArrayFunctionSizeZero",
        "testcreateJsonFunctionObject",
        "testcreateJsonFunctionArray",
        "testcreateJsonFunctionInt",
        "testcreateJsonFunctionDouble",
        "testcreateJsonFunctionBool",
        "testcreateJsonFunctionCharPtr",
        "testcreateJsonFunctionCharArray",
        "testcreateJsonFunctionNull",
        "testChangeJsonArraySizeFuntionIncrease",
        "testChangeJsonArraySizeFuntionDecrease",
        "testSetArrayValueFunction",
        "testSetArrayValueFunctionOutOfBounds",
        "testGetArrayValueFunction",
        "testJsonStringifyFunction",
        "testJsonParseFunction",
    };

    size_t failedTests = 0;
    size_t numTests = sizeof(fun_ptr) / sizeof(bool (*)());

    for (size_t i = 0; i < numTests; i++) {
        if (!fun_ptr[i]()) {
            printf("Test %s failed\n", nameTests[i]);
            failedTests++;
        } else {
            printf("Test %s passed\n", nameTests[i]);
        }
    }
    printf("%ld/%ld tests passed\n", numTests - failedTests, numTests);
    if (!failedTests) {
        printf("All tests passed! Nice job\n");
        return 0;
    }

    return (int) failedTests;   
}