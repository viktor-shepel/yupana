#include "yupana.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
 *  Formal grammar in EBNF:
 *
 *  expression ::= sum
 *  sum ::= difference | difference "+" sum
 *  difference ::= product | difference "-" product
 *  product ::= fraction | fraction "*" product
 *  fraction ::= term | fraction "/" term
 *  term = whitespace* base whitespace*
 *  base ::= "(" expression ")" | number
 *  number ::= [+-]? (digit* ".")? digit+
 *
 *  whitespace ::= " " | "\t" | "\n" | "\v" | "\f" | "\r"
 *  digit ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 *
 */

typedef struct {
  bool valid;
  double value;
  const char* context;
  const char* reason;
} expression_t;

static expression_t valid_expression(double value, const char* context) {
  return (expression_t) { true, value, context, NULL };
}

static expression_t invalid_expression(
    const char* reason,
    const char* context
) {
  return (expression_t) { false, strtod("NaN", NULL), context, reason };
}

static const char* of_invalid_one(expression_t left, expression_t right) {
  const char* left_context = left.valid ? NULL : left.context;
  const char* right_context = right.valid ? NULL : right.context;

  return left_context ? left_context : right_context;
}

static const char* after_whitespaces(const char* symbols) {
  return isspace(*symbols) ? after_whitespaces(symbols + 1) : symbols;
}

/* begin: parsers */

static expression_t eval_number(const char* symbols) {
  char* symbols_after_number = NULL;
  double number = strtod(symbols, &symbols_after_number);
  return (symbols_after_number == symbols)
         ? invalid_expression("malformed number", symbols)
         : valid_expression(number, symbols_after_number);
}

static expression_t eval_expresison(const char* symbols);

static expression_t eval_base(const char* symbols) {
  // base ::= "(" expression ")" | number
  if (*symbols == '(') {
    expression_t expression = eval_expresison(symbols + 1);
    return expression.valid && *expression.context == ')'
           ? valid_expression(expression.value, expression.context + 1)
           : invalid_expression(
               "can't evaluate expression inside parentheses",
               symbols
           );
  }

  return eval_number(symbols);
}

static expression_t eval_term(const char* symbols) {
  // term = whitespace* base whitespace*
  expression_t base = eval_base(after_whitespaces(symbols));
  return base.valid
         ? valid_expression(base.value, after_whitespaces(base.context))
         : invalid_expression(base.reason, base.context);
}

static expression_t eval_fraction(const char* symbols) {
  // fraction ::= term | fraction "/" term
  expression_t left = eval_term(symbols);
  if (*left.context != '/')
    return left;

  while (*left.context == '/' && left.valid) {
    expression_t right = eval_term(left.context + 1);
    left = right.valid
           ? valid_expression(left.value / right.value, right.context)
           : invalid_expression(
               "division operator has malformed arguments",
               right.context
           );
  };

  return left;
}

static expression_t eval_product(const char* symbols) {
  // product ::= fraction | fraction "*" product
  expression_t left = eval_fraction(symbols);
  if (*left.context != '*')
    return left;

  expression_t right = eval_product(left.context + 1);

  return (left.valid && right.valid)
         ? valid_expression(left.value * right.value, right.context)
         : invalid_expression(
             "product operator has malformed arguments",
             of_invalid_one(left, right)
         );
}

static expression_t eval_difference(const char* symbols) {
  // difference ::= product | difference "-" product
  expression_t left = eval_product(symbols);
  if (*left.context != '-')
    return left;

  while (*left.context == '-' && left.valid) {
    expression_t right = eval_product(left.context + 1);
    left = right.valid
           ? valid_expression(left.value - right.value, right.context)
           : invalid_expression(
               "substraction operator has malformed arguments",
               right.context
           );
  };

  return left;
}

static expression_t eval_sum(const char* symbols) {
  // sum ::= difference | difference "+" sum
  expression_t left = eval_difference(symbols);
  if (*left.context != '+')
    return left;

  expression_t right = eval_sum(left.context + 1);

  return (left.valid && right.valid)
         ? valid_expression(left.value + right.value, right.context)
         : invalid_expression(
             "addition operator has malformed arguments",
             of_invalid_one(left, right)
         );
}

static expression_t eval_expresison(const char* symbols) {
  return eval_sum(symbols);
}

/* end: parsers */

static eval_error_t eval_error(
    const char* description,
    const char* context
) {
  return (eval_error_t) { description, context };
}

static eval_error_t nil_error() {
  return (eval_error_t) { NULL, NULL };
}

static optional_number_t valid_number(double value) {
  return (optional_number_t) { true, value, nil_error() };
}

static optional_number_t invalid_number(eval_error_t error) {
  return (optional_number_t) { false, strtod("NaN", NULL), error };
}

optional_number_t eval(const char* expression) {
  expression_t number = eval_expresison(expression);
  const char* reason = number.valid ? "can't evaluate expression"
                                    : number.reason;
  return number.valid && strlen(number.context) == 0
         ? valid_number(number.value)
         : invalid_number(eval_error(reason, number.context));
}
