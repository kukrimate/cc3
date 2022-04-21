int printf(const char *, ...);

// Complete struct
struct foo {
    int a;
    short b;
};

// Indirectly complete a struct through a typedef
typedef struct bar bar_t;

struct bar {
    int a;
    int b;
};

int main(void)
{
    struct foo foo = { 0xdeadbeef, 0xf00d };
    printf("foo.a = %x\n", foo.a);
    printf("foo.b = %hx\n", foo.b);

    bar_t bar = { 1, 65537 };
    printf("bar.a = %d\n", bar.a);
    printf("bar.b = %d\n", bar.b);
}
