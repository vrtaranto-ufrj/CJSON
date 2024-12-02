#pragma once

#include "jsonv2.h"

JsonKeyValuePair * findKeyValuePair(JsonObject *json_obj, const char *key);  
void setKeyValuePair(JsonObject *json_obj, const char *key, Json *value);
void setArrayValue(JsonArray *json_array, size_t index, Json *value);

void addKeyValuePair(JsonObject *json_obj, const char *key, Json *value);

void setNull(JsonValue *json);
void setInt(JsonValue *json, int64_t val_int);
void setDouble(JsonValue *json, double val_double);
void setBool(JsonValue *json, bool val_bool);
void setString(JsonValue *json, char * val_string);
void setObject(JsonValue *json, JsonObject *obj_ptr);
void setArray(JsonValue *json, JsonArray *array_ptr);
