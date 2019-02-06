int addInt(int n, int m) {
    return n + m;
}

int (*functionFactory(int n))(int, int) {
    int (*functionPtr)(int,int);
    functionPtr = &addInt;
    return functionPtr;
}

 void main() {
  (*(functionFactory(1)))(1, 2);
}
