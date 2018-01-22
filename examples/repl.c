#define _GNU_SOURCE
#include "../src/yupana.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* serialize(optional_number_t number);

int main(void) {
  char input[500];
  char* buffer = input;
  size_t line_length;

  while (getline(&buffer, &line_length, stdin) != EOF) {
    printf("%s\n", serialize(eval(buffer)));
  }
  
  return 0;
}

const char* id(const char* text) {
  return text;
}

const char* turn_green(const char* text) {
  const char* prefix = "\x1b[32m";
  const char* suffix = "\x1b[0m";
  const size_t size = strlen(prefix) + strlen(text) + strlen(suffix);
  return strcat(strcat(strcat(malloc(size), prefix), text), suffix);
}

const char* turn_red(const char* text) {
  const char* prefix = "\x1b[31m"; // https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
  const char* suffix = "\x1b[0m";
  const size_t size = strlen(prefix) + strlen(text) + strlen(suffix);
  return strcat(strcat(strcat(malloc(size), prefix), text), suffix);
}

const char* serialize_number(double number) {
  char* string = malloc(50);
  int ok = sprintf(string, "%g", number);
  
  return ok ? string : (free(string), NULL);
}

const char* serialize_error(eval_error_t error) {
  char* string = malloc(500);
  int ok = sprintf(
    string,
    "{ description: \"%s\"; context: \"%s\"; }",
    error.description,
    error.context
  );
  
  return ok ? string : (free(string), NULL);
}

const char* serialize(optional_number_t number) {
  typedef const char* (*termcap_decorator_t)(const char*);

  termcap_decorator_t colorize_value = number.valid ? turn_green : id;
  termcap_decorator_t colorize_error = number.valid ? id : turn_red;

  const char* valid = number.valid ? "true" : "false";
  const char* value = colorize_value(serialize_number(number.value));
  const char* error = colorize_error(serialize_error(number.error));
  const char* fmt = "{ valid: %s; value: %s; error: %s; }";
  const size_t size = strlen(valid) + strlen(value) + strlen(error) + strlen(fmt);

  char* string = malloc(size);
  int ok = sprintf(string, fmt, valid, value, error);

  return ok ? string : (free(string), NULL);
}
