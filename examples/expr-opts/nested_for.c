/*
 * KLEE test cases to show expression optimisation
 */

#include "klee/klee.h"

int add_or(int x, int y, int z) {

  for (int i = z; i < 1000; i+=x) {
    if ( z < x && ((z | 123456) & y) < x) {
      for (int j = x; j < i; j++) {
        int a = x | y;
        int b = x ^ y;

        if ((a | b) == 0 && y == y)
          return i;
      }
    }
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
