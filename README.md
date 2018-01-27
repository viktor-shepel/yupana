[![Build Status](https://travis-ci.org/viktor-shepel/yupana.svg?branch=master)](https://travis-ci.org/viktor-shepel/yupana)
## Overview
Yupana is a C library that evaluates arithmetic expression.

## Features
* single header/source file
* C99 standard compliant
* zero external dependencies
* thread safe

## Getting Started
1. create folder for a project and clone `yupana` to it
```bash
mkdir app
cd app
git clone https://github.com/viktor-shepel/yupana.git
```
2. create `main.c` file with next content
```c
#include <yupana.h>
#include <stdio.h>

int main() {
  optional_number_t number = eval("1 + 2 / 5");

  printf("eval(\"1 + 2 / 5\") == %g\n", number.value);
  
  return 0;
}
```
3. compile project
```bash
gcc -Wall -std=c99 -c yupana/src/yupana.c -o yupana.o
gcc -Wall -std=c99 -I yupana/src/ -c main.c -o main.o
gcc main.o yupana.o -o main
```
4. run binary
```bash
./main
eval("1 + 2 / 5") == 1.4
```
5. have fun :smiley:
## Documentation

### eval_error_t
```c
typedef struct {
  const char* description;
  const char* context;
} eval_error_t;
```
Is a data structure that represents evaluation error. It maintains human readable description of error and place of it's origin as `description`/`context` fields respectively.
```c
const char* expression = "2 / :five:";
eval_error_t error = eval(expression).error;

error.description; // points to string "division operator has malformed arguments"
error.context == &expression[4]; // true

/*
 *  Graphical interpretaton of error context
 *
 *  eval_error_t erro = eval("2 / :five:").error;
 *                                ^
 *                                |
 *  error.context  ---------------+
 */

```
### optional_number_t
```c
typedef struct {
  bool valid;
  double value;
  eval_error_t error;
} optional_number_t;
```
Is a data structure that represents evaluation that might fail.

Valid number represented as `{ .valid = true, .value = <actual value>, .erro = { NULL, NULL } }`.

Invalid number represented as `{ .valid = false, .value = NaN, .error = <computation flaw> }`.
```c
optional_number_t number = eval("2 + (8 / 4)");
number.valid == true;
number.value == 4.0;
number.error.description == NULL;
number.error.context == NULL;

/* VS */

optional_number_t number = eval("2 + (8 / /)");
number.valid == false;
number.value == NAN; // don't compare in such way, it is implementation specific
number.error.description != NULL;
number.error.context != NULL;
```
### eval
```c
optional_number_t eval(const char* expression);
```
Is a function that evaluates text with arithmetic expression `expression`. It returns optional number wich encapsulates double precision value for well formed expression or error for malformed one.
```c
optional_number_t number = eval("0.1 + 0.2");
number.valid == true;
number.value == 0.3;

optional_number_t number = eval("NaN");
number.valid == true;
number.value == NAN; // if your platform supports NAN

optional_number_t number = eval("一 + 三");
number.valid == false;
number.value == NAN; // don't expect it to be 4.0

optional_number_t number = eval("I + III");
number.valid == false;
number.value == NAN; // same as above unless it runs on scholar pc
```
## Examples
navigate to `yupana` folder and build examples (play with/explore it)
```bash
cd yupana
make
```

`repl` - for interactive exploration

![repl](https://raw.githubusercontent.com/viktor-shepel/yupana/master/examples/repl.gif)
`calc` - as pipe command
<img alt="calc" src="https://raw.githubusercontent.com/viktor-shepel/yupana/master/examples/calc.gif" height="500px" width="100%">
