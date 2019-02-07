struct A;
struct A;

void main () {
  struct A *p;
}

struct A;

struct A {
  int y;
};

struct B {
  struct A* p;
  struct A {
    struct C {
      int x;
    } b;
  } a;
};


void foo () {
  struct B b;
  b.p->y;
  b.a.b.x;
}
