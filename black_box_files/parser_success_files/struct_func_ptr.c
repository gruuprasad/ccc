void main() {
  struct S {
    int (*f)(int);
    } s;
  s.f(5);
}
