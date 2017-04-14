#include "minijson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISSPACE(c) ((char)(c) == ' ' || (char)(c) == '\n')
#define SKIPSPACE(p) while(ISSPACE(*p))++p


// internal use
parse_status parse_object(char *, char **);
parse_status parse_array(char *, char **);
parse_status parse_number(char *, char **);
parse_status parse_string(char *, char **);

parse_status skip_object(char **);
parse_status skip_array(char **);
parse_status skip_number(char **);
parse_status skip_string(char **);
parse_status skip_value(char **);


parse_status skip_value(char **top)
{
  char *orig = *top;
  char *p;
  parse_status ret;

  switch(**top) {
    case '{':
      ret = skip_object(top);
      break;
    case '[':
      ret = skip_array(top);
      break;
    case '"':
      ret = skip_string(top);
      break;
    case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      ret = skip_number(top);
      break;
    case 't': // true
      if(!strncmp(*top, "true", 4)) {
        ret = PARSE_SUCCEEDED;
        *top += 3;
      }
      else {
        ret = PARSE_FAILED_TRUE;
      }
      break;
    case 'f': // false
      if(!strncmp(*top, "false", 5)) {
        ret = PARSE_SUCCEEDED;
        *top += 4;
      }
      else {
        ret = PARSE_FAILED_FALSE;
      }
      break;
    case 'n': // null
      if(!strncmp(*top, "null", 4)) {
        ret = PARSE_SUCCEEDED;
        *top += 3;
      }
      else {
        ret = PARSE_FAILED_NULL;
      }
      break;
    default:
      break;
  }

  if (ret != PARSE_SUCCEEDED) {
    *top = orig;
    return ret;
  }

  return PARSE_SUCCEEDED;
}


parse_status skip_object(char **top) {
  char *p, *orig = *top;
  parse_status ret;
  if((ret = parse_object(*top, &p)) != PARSE_SUCCEEDED) {
    *top = orig;
    return ret;
  }
  *top = p;
  return PARSE_SUCCEEDED;
}

parse_status skip_array(char **top) {
  char *p, *orig = *top;
  parse_status ret;
  if((ret = parse_array(*top, &p)) != PARSE_SUCCEEDED) {
    *top = orig;
    return ret;
  }
  *top = p;
  return PARSE_SUCCEEDED;
}

parse_status skip_string(char **top) {
  char *p, *orig = *top;
  parse_status ret;
  if((ret = parse_string(*top, &p)) != PARSE_SUCCEEDED) {
    *top = orig;
    return ret;
  }
  *top = p;
  return PARSE_SUCCEEDED;
}

parse_status skip_number(char **top) {
  char *p, *orig = *top;
  parse_status ret;
  if((ret = parse_number(*top, &p)) != PARSE_SUCCEEDED) {
    *top = orig;
    return ret;
  }
  *top = p;
  return PARSE_SUCCEEDED;
}


parse_status parse_object(char *beg, char **end)
{
  if(*beg != '{')
    return PARSE_FAILED_OBJECT;

  char *p = beg;
  char *tmp;
  parse_status ret;

  p++; // next to '{'
  while(*p != '}') {
    // key
    SKIPSPACE(p);
    if((ret = skip_string(&p)) != PARSE_SUCCEEDED)
      return ret;
    p++; // next to closing '"'
    SKIPSPACE(p);
    if(*p != ':')
      return PARSE_FAILED_OBJECT;
    p++; // next to ':'
    SKIPSPACE(p);

    // value
    if((ret = skip_value(&p)) != PARSE_SUCCEEDED)
      return ret;
    p++;
    SKIPSPACE(p);

    // comma
    if(*p != ',' && *p != '}') {
      return PARSE_FAILED_OBJECT;
    }
    if(*p == ',') {
      p++;
      SKIPSPACE(p);
    }
  }
  *end = p;
  return PARSE_SUCCEEDED;
}

parse_status parse_array(char *beg, char **end)
{
  if(*beg != '[')
    return PARSE_SUCCEEDED;

  char *p = beg;
  parse_status ret;

  p++; // next to '['
  while(*p != ']') {
    SKIPSPACE(p);
    if((ret = skip_value(&p)) != PARSE_SUCCEEDED)
      return ret;

    p++;
    SKIPSPACE(p);

    if(*p != ',' && *p != ']') {
      return PARSE_FAILED_ARRAY;
    }
    if(*p == ',') {
      p++;
      SKIPSPACE(p);
    }
  }
  *end = p;
  return PARSE_SUCCEEDED;
}

parse_status parse_string(char *beg, char **end) // "[^"]*"
{
  if(*beg != '"')
    return PARSE_FAILED_STRING;

  char *p = beg;
  while(*(++p) != '"')
    if(*p == '\\') p++;

  *end = p;
  return PARSE_SUCCEEDED;
}

parse_status parse_number(char *beg, char **end) // [0-9]+
{
  char *p;
  for(p = beg; ('0'<=(*p) && (*p)<='9') || *p == '-'; ++p);
  *end = p-1;
  if(beg > *end)
    return PARSE_FAILED_NUMBER;
  else
    return PARSE_SUCCEEDED;
}



parse_status json_parse(const char *str, json_value_t *res)
{
  char *p = (char*)str;
  int ret = PARSE_SUCCEEDED;
  SKIPSPACE(p);

  switch(*p) {
    case '{':
      res->type = JSON_OBJECT;
      break;
    case '[':
      res->type = JSON_ARRAY;
      break;
    case '\"':
      res->type = JSON_STRING;
      break;
    case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      res->type = JSON_NUMBER;
      break;
    case 't':
      res->type = JSON_TRUE;
      break;
    case 'f':
      res->type = JSON_FALSE;
      break;
    case 'n':
      res->type = JSON_NULL;
      break;
    default:
      ret = PARSE_FAILED;
  }
  if(ret == PARSE_SUCCEEDED) res->value = p;
  return ret;
}

parse_status json_get_object(json_value_t json, const char *key, json_value_t *res)
{
  if(json.type != JSON_OBJECT)
    return PARSE_FAILED_TYPE_MISSMATCH;

  char *p = json.value;
  char *tmp;
  int key_len = strlen(key);
  parse_status ret;

  p++;
  while(1) {
    SKIPSPACE(p);

    if(*p == '}') {
      return PARSE_FAILED_NOT_FOUND_KEY;
    }

    if(*p == '\"' && *(p+key_len+1) == '\"' && !strncmp(p+1, key, key_len)) { // found key
      tmp = p+key_len+2; // next to closing '\"'
      SKIPSPACE(tmp);
      if(*tmp != ':') return PARSE_FAILED_OBJECT;
      tmp++;
      SKIPSPACE(tmp);
      return json_parse(tmp, res);
    }

    // skip until next key
    if((ret = skip_string(&p)) != PARSE_SUCCEEDED) {
      return ret;
    }
    p++; // next to closing key '"'
    SKIPSPACE(p);
    if (*p != ':')
      return PARSE_FAILED_OBJECT;
    p++; // next to ':'
    SKIPSPACE(p);

    if((ret = skip_value(&p)) != PARSE_SUCCEEDED) {
      return ret;
    }
    p++;
    SKIPSPACE(p);

    // comma
    if(*p == ',') {
      p++;
    }
    SKIPSPACE(p);
  }
}

parse_status json_get_array(json_value_t json, int idx, json_value_t *res)
{
  if(json.type != JSON_ARRAY)
    return PARSE_FAILED_TYPE_MISSMATCH;

  char *p = json.value;
  char *tmp;
  parse_status ret;

  p++;
  while(idx--) {
    SKIPSPACE(p);
    skip_value(&p);
    p++;
    SKIPSPACE(p);
    if(*p != ',')
      return PARSE_FAILED_ARRAY;
    p++;
  }
  SKIPSPACE(p);
  return json_parse(p, res);
}

parse_status json_get_number(json_value_t json, long long *val)
{
  if(json.type != JSON_NUMBER)
    return PARSE_FAILED_TYPE_MISSMATCH;

  char *end;
  char buf[32];
  int i;
  parse_status ret;
  if((ret = parse_number(json.value, &end)) != PARSE_SUCCEEDED)
    return ret;
  for(i = 0; i <= end - json.value; i++) buf[i] = *(json.value+i);
  *val = atol(buf);
  return PARSE_SUCCEEDED;
}

parse_status json_get_string(json_value_t json, char *val)
{
  if(json.type != JSON_STRING)
    return PARSE_FAILED_TYPE_MISSMATCH;

  char *end;
  int i;
  parse_status ret;
  if((ret = parse_string(json.value, &end)) != PARSE_SUCCEEDED) 
    return ret;
  for(i = 0; i < end - json.value - 1; i++) *(val+i) = *(json.value+i+1);
  *(val+i) = '\0';
  return PARSE_SUCCEEDED;
}


void puterror(parse_status status) {
  fprintf(stderr, "error: ");
  switch(status) {
    case PARSE_SUCCEEDED:
      fprintf(stderr, "PARSE_SUCCEEDED");
      break;
    case PARSE_FAILED:
      fprintf(stderr, "PARSE_FALIED");
      break;
    case PARSE_FAILED_OBJECT:
      fprintf(stderr, "PARSE_FALIED_OBJECT");
      break;
    case PARSE_FAILED_ARRAY:
      fprintf(stderr, "PARSE_FALIED_ARRAY");
      break;
    case PARSE_FAILED_STRING:
      fprintf(stderr, "PARSE_FALIED_STRING");
      break;
    case PARSE_FAILED_NUMBER:
      fprintf(stderr, "PARSE_FALIED_NUMBER");
      break;
    case PARSE_FAILED_TRUE:
      fprintf(stderr, "PARSE_FAILED_TRUE");
      break;
    case PARSE_FAILED_FALSE:
      fprintf(stderr, "PARSE_FAILED_FALSE");
      break;
    case PARSE_FAILED_NULL:
      fprintf(stderr, "PARSE_FAILED_NULL");
      break;
    case PARSE_FAILED_NOT_FOUND_KEY:
      fprintf(stderr, "PARSE_FALIED_NOT_FOUND_KEY");
      break;
    case PARSE_FAILED_TYPE_MISSMATCH:
      fprintf(stderr, "PARSE_FALIED_TYPE_MISSMATCH");
      break;
  }
  fprintf(stderr, "\n");
}
