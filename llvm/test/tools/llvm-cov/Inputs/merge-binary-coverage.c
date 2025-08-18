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