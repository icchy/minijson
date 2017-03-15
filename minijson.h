#ifndef _INCLUDE_MINIJSON_H
#define _INCLUDE_MINIJSON_H

typedef enum {
  PARSE_SUCCEEDED = 0,
  PARSE_FAILED,
  PARSE_FAILED_OBJECT,
  PARSE_FAILED_ARRAY,
  PARSE_FAILED_STRING,
  PARSE_FAILED_NUMBER,
  PARSE_FAILED_TRUE,
  PARSE_FAILED_FALSE,
  PARSE_FAILED_NULL,
  PARSE_FAILED_NOT_FOUND_KEY,
  PARSE_FAILED_TYPE_MISSMATCH,
} parse_status;

typedef enum {
  JSON_STRING,
  JSON_NUMBER,
  JSON_OBJECT,
  JSON_ARRAY,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL,
} json_value;

typedef struct {
  json_value type;
  char *value;
} json_value_t;

parse_status json_parse(const char*, json_value_t*);
parse_status json_get_object(json_value_t, const char*, json_value_t*);
parse_status json_get_array(json_value_t, int, json_value_t*);
parse_status json_get_number(json_value_t, long long*);
parse_status json_get_string(json_value_t, char*);

void puterror(parse_status);

#endif // _INCLUDE_MINIJSON_H
