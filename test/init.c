/*
 * C initializers are genuinely making me nocturnal
 */

int printf(const char *, ...);

int main(void)
{
    struct { int a, b; } s = {{5,6}};

    printf("%d %d\n", s.a, s.b);
}
