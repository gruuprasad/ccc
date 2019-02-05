int fac(int);
int main() {
return fac(4);
}
int fac(int a) {
  if(a < 1)
    return 1;
  else
    return a * fac (a - 1);
}
