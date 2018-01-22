#define _GNU_SOURCE
#include "../src/yupana.h"
#include <stdio.h>

int main(void) {
  char input[500];
  char* buffer = input;
  size_t input_size;

  while (getline(&buffer, &input_size, stdin) != EOF) {
    optional_number_t number = eval(buffer);
    if (number.valid) {
        printf("%g\n", number.value);
    }
  }
  
  return 0;
}
