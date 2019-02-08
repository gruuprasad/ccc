struct A {
  int y;
};

struct B {
  struct A i;
  struct A {
    int x;
    struct A *p;
  } a;
  struct A j;
};

void main {
  struct A a;
  struct B b;
  b.a.p->x;
  a.y;
  b.a.x;
  b.i.y;
  b.j.x;
}
