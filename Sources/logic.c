#include "../Include/logic.h"
#include <stdlib.h>

static double parse_number(const char **s) {
  /* Returneaza numarul */
  double result = strtod(*s, (char **)s);
  return result;
}

// Declarat inainte pentru recursivitate
static double parse_op1(const char **s);

static double parse_op3(const char **s) {
  /* Parseaza parantezele */
  if (**s == '(') {
    (*s)++; // Skip '('
    double val = parse_op1(s);
    if (**s == ')')
      (*s)++; // Skip ')'
    return val;
  } else {
    // 's' nu este paranteza
    return parse_number(s);
  }
}

static double parse_op2(const char **s) {
  /* Parseaza operatiile: '*', '\', '\\', 'mod' */
  double val = parse_op3(s);
  while (1) {
    if (**s == '*') {
      (*s)++;
      val *= parse_op3(s);
    } else if (**s == '/') {
      (*s)++;
      double denom = parse_op3(s);
      val = (denom != 0) ? val / denom : 0;
    } else {
      break;
    }
  }
  return val;
}

static double parse_op1(const char **s) {
  /* Parseaza operatiile: '+', '-' */
  double val = parse_op2(s);
  while (1) {
    if (**s == '+') {
      (*s)++;
      val += parse_op2(s);
    } else if (**s == '-') {
      (*s)++;
      val -= parse_op2(s);
    } else {
      break;
    }
  }
  return val;
}

double evaluate_expression(const char *expr) {
  /* Parcurgem expresia primita ca input recursiv, respectand ordinul
   * operatorilor: 
   * Ordin 1: '+', '-'; 
   * Ordin 2: '*', '/', '//', 'mod' 
   * Ordin 3: '(', ')' */
  const char *p = expr;
  return parse_op1(&p);
}
