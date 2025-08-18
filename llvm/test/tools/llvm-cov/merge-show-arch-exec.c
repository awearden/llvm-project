// RUN: clang -O0 -fprofile-instr-generate -fcoverage-mapping -DTOGGLE %s \
// RUN:       -o %t.toggle
// RUN: %t.toggle
// RUN: cp default.profraw %t.toggle.profraw

//------------------ build & run variant WITHOUT TOGGLE ----------------------//
// RUN: clang -O0 -fprofile-instr-generate -fcoverage-mapping %s \
// RUN:       -o %t.notoggle
// RUN: %t.notoggle
// RUN: cp default.profraw %t.notoggle.profraw 

//------------------ merge profiles & show coverage --------------------------//
// RUN: llvm-profdata merge --object-aware-hashing=%t.notoggle %t.notoggle.profraw\
// RUN:     --object-aware-hashing=%t.toggle   %t.toggle.profraw \
// RUN:     -o %t.profdata
//
// RUN: llvm-cov show -instr-profile=%t.profdata --object=%t.toggle --object=%t.notoggle --merge-binary-coverage --show-arch-executables | FileCheck %s --match-full-lines

// CHECK:   64|      2|int main() {
//CHECK-NEXT:   65|      2|  int a = 1;
//CHECK-NEXT:   66|      2|  int b = 2;
//CHECK-NEXT:   67|      2|  int res = 0;
//CHECK-NEXT:   68|      2|#if defined(TOGGLE)
//CHECK-NEXT:   69|      1|  res = a + b;
//CHECK-NEXT:   70|      1|#else
//CHECK-NEXT:   71|      1|  res = b - a;
//CHECK-NEXT:   72|      1|#endif
//CHECK-NEXT:   73|      2|  return 0;
//CHECK-NEXT:   74|      2|}
//CHECK-NEXT:  ------------------
//CHECK-NEXT:  | main:
//CHECK-NEXT:        -x86_64
//CHECK-NEXT:        -{{.*}}toggle:
//CHECK-NEXT:  |   64|      1|int main() {
//CHECK-NEXT:  |   65|      1|  int a = 1;
//CHECK-NEXT:  |   66|      1|  int b = 2;
//CHECK-NEXT:  |   67|      1|  int res = 0;
//CHECK-NEXT:  |   68|      1|#if defined(TOGGLE)
//CHECK-NEXT:  |   69|      1|  res = a + b;
//CHECK-NEXT:  |   70|       |#else
//CHECK-NEXT:  |   71|       |  res = b - a;
//CHECK-NEXT:  |   72|       |#endif
//CHECK-NEXT:  |   73|      1|  return 0;
//CHECK-NEXT:  |   74|      1|}
//CHECK-NEXT:  ------------------
//CHECK-NEXT:  | main:
//CHECK-NEXT:        -x86_64
//CHECK-NEXT:        -{{.*}}notoggle:
//CHECK-NEXT:  |   64|      1|int main() {
//CHECK-NEXT:  |   65|      1|  int a = 1;
//CHECK-NEXT:  |   66|      1|  int b = 2;
//CHECK-NEXT:  |   67|      1|  int res = 0;
//CHECK-NEXT:  |   68|       |#if defined(TOGGLE)
//CHECK-NEXT:  |   69|       |  res = a + b;
//CHECK-NEXT:  |   70|       |#else
//CHECK-NEXT:  |   71|      1|  res = b - a;
//CHECK-NEXT:  |   72|      1|#endif
//CHECK-NEXT:  |   73|      1|  return 0;
//CHECK-NEXT:  |   74|      1|}
//CHECK-NEXT:  ------------------



int main() {
  int a = 1;
  int b = 2;
  int res = 0;
#if defined(TOGGLE)
  res = a + b;
#else
  res = b - a;
#endif
  return 0;
}