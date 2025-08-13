// RUN: clang -O0 -fprofile-instr-generate -fcoverage-mapping %s \
// RUN:       -o %t.bin1
// RUN: %t.bin1
// RUN: cp default.profraw %t.bin1.profraw

//------------------ build & run variant WITHOUT TOGGLE ----------------------//
// RUN: clang -O0 -fprofile-instr-generate -fcoverage-mapping %s \
// RUN:       -o %t.bin2
// RUN: %t.bin2
// RUN: cp default.profraw %t.bin2.profraw 

//------------------ merge profiles & show coverage --------------------------//
// RUN: llvm-profdata merge --object-aware-hashing=%t.bin2 %t.bin2.profraw\
// RUN:     --object-aware-hashing=%t.bin1   %t.bin1.profraw \
// RUN:     -o %t.profdata
//
// RUN: llvm-cov show -instr-profile=%t.profdata --object=%t.bin1 --object=%t.bin2 --merge-binary-coverage --show-arch-executables | FileCheck %s --match-full-lines

// CHECK:   40|      2|int main() {
// CHECK-NEXT:   41|      2|  return 0;
// CHECK-NEXT:   42|      2|}
// CHECK-NEXT:  ------------------
// CHECK-NEXT:  | main:
// CHECK-NEXT:        -x86_64
// CHECK-NEXT:        -{{.*}}bin1:
// CHECK-NEXT:  |   40|      1|int main() {
// CHECK-NEXT:  |   41|      1|  return 0;
// CHECK-NEXT:  |   42|      1|}
// CHECK-NEXT:  ------------------
// CHECK-NEXT:  | main:
// CHECK-NEXT:        -x86_64
// CHECK-NEXT:        -{{.*}}bin2:
// CHECK-NEXT:  |   40|      1|int main() {
// CHECK-NEXT:  |   41|      1|  return 0;
// CHECK-NEXT:  |   42|      1|}
// CHECK-NEXT:  ------------------



int main() {
  return 0;
}