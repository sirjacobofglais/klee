/*
 * KLEE test cases to show expression optimisation
 */

#include "klee/klee.h"

int add_or(int x, int y, int z) {

  int a = x | y;
  int b = x ^ y;

  if ((a | b) == 0 && y == y)
     return 0;
  else return 1;
} 

int main() {
  int a;
  klee_make_symbolic(&a, sizeof(a), "a");

  int b;
  klee_make_symbolic(&b, sizeof(b), "b");

  int c;
  klee_make_symbolic(&c, sizeof(c), "c");
  return add_or(a, b, c);
} 
