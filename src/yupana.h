#ifndef YUPANA_H
#define YUPANA_H

#include <stdbool.h>

typedef struct {
  const char* description;
  const char* context;
} eval_error_t;

typedef struct {
  bool valid;
  double value;
  eval_error_t error;
} optional_number_t;

optional_number_t eval(const char* expression);

#endif
