#include <stdio.h>

struct foo {
  int a, b;

  union {
    struct {
      int c, d;
    } small;

    struct {
      long e, f;
    } big;
  };
};

struct foo f = {};
char ch = {};

int main(void)
{
  printf ("%ld %ld\n", sizeof (struct foo), (char *) &ch - (char *) &f);
}
