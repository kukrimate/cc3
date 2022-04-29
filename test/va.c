int puts(const char *);
int printf(const char *, ...);

void vprint(int cnt, __builtin_va_list ap)
{
    while (cnt--)
        printf("%s\n", __builtin_va_arg(ap, const char *));
}

void print(int cnt, ...)
{
    printf("print(%d, ...)\n", cnt);
    __builtin_va_list ap;
    __builtin_va_start(ap, cnt);
    vprint(cnt, ap);
    __builtin_va_end(ap);
}

int main(void)
{
    print(0);
    print(1, "foo");
    print(2, "foo", "bar");
    print(3, "foo", "bar", "lol");
}
