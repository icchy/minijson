#include <stdio.h>
#include <stdlib.h>

#include "minijson.h"

extern char _binary_json_start[];
extern char _binary_json_end[];
extern char _binary_json_size[];

__attribute__((constructor)) int init() {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  setbuf(stdin, NULL);
}

int main() {
  json_value_t root, object, sub, val, array;
  long long integer;
  char *buf = malloc(32);
  parse_status ret;

  if((ret = json_parse(_binary_json_start, &root))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  // test object key
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


  // test array index
  if((ret = json_get_object(root, "array", &array))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  if((ret = json_get_array(array, 2, &val))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  if((ret = json_get_string(val, buf))) {
    puterror(ret);
    exit(EXIT_FAILURE);
  }

  printf("root.array[2]: \"%s\"\n", buf);


  return 0;
}
