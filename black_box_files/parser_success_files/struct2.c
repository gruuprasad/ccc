struct A {
  int y;
};

struct B {
  struct A i;
  struct A *k;
  struct A {
    int x;
    struct A *p;
  } a;
  struct A j;
  struct A l;
};

void main {
  struct A a;
  struct B b;
  b.a.p->x;
  a.y;
  b.a.x;
  b.i.y;
  b.j.x;
  b.k->y;
  b.l.p->p->x;
}
