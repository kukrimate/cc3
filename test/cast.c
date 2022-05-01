int printf(const char *, ...);

struct foo { int a; };

int main(void)
{
    struct foo f = {5};
    void *vp = &f;
    printf("%d\n", ((struct foo *) vp)->a);
}
