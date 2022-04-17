struct foo {
    int a;
    long b;
};

struct bar {
    _Bool a;
    int b;
};

union foobar {
    float f;
    double d;
};

enum baz {
    ONE,
    TWO
};

int printf(const char *, ...);

int main(void)
{
    struct foo foo;
    struct bar bar;

    foo.a = 1;
    foo.b = 2;

    bar.a = 1;
    bar.b = 2;
}
