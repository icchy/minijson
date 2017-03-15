# minijson
tiny json parser (written for baremetal)

## Features
 * without malloc
 * only for getting value from json (listing value is not supported)

## supported value type
 * object
 * array
 * string
 * number (integer only)
 * true
 * false
 * null

## required function
 * fprintf (error output)
 * strncmp, strlen
 * atol

## api
 * `parse_status json_parse(const char*, json_value_t*)`
 * `parse_status json_get_object(json_value_t, const char*, json_value_t*)`
 * `parse_status json_get_array(json_value_t, int, json_value_t*)`
 * `parse_status json_get_number(json_value_t, long long*)`
 * `parse_status json_get_string(json_value_t, char*)`

## usage
```c
#include <stdio.h>
#include <stdlib.h>

#include "minijson.h"

#define HERE(...) #__VA_ARGS__ "\n";

int main() {
  char json_data[] = HERE(
    {
      "number": 1,
      "string": "test",
      "true": true,
      "false": false,
      "null": null,
      "object": {
        "subkey": {
          "foo": 2,
          "bar": "3"
        },
        "subkey2": "aaaa"
      },
      "array": [4, 100, "bbbb"]
    }
  );

  json_value_t root, object, sub, val;
  long long integer;
  parse_status ret;

  if((ret = json_parse(json_data, &root))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  if((ret = json_get_object(root, "object", &object))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }
  if((ret = json_get_object(object, "subkey", &sub))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  if((ret = json_get_object(sub, "foo", &val))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  if((ret = json_get_number(val, &integer))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  printf("root.object.subkey.foo: %lu\n", integer);

  return 0;
}
```
