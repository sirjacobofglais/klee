/*
 * KLEE test cases to show expression optimisation
 */

#include "klee/klee.h"

int add_or(int x, int y, int z) {
  int a = 0;
  int b = 0;
  for (int i = z; i < y; i+=x) {
    a = x | b;
    b = a ^ y;

    if ((a | i) == z || y == b)
      return i;
  }
  return 1;
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
