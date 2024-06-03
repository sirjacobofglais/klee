/*
 * KLEE test cases to show expression optimisation
 */

#include "klee/klee.h"

int equality(int x) {

  if ((x - 1) == 0)
     return 0;
  else return 1;
} 

int main() {
  int a;
  klee_make_symbolic(&a, sizeof(a), "a");
  return equality(a);
} 
