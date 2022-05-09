typedef __builtin_va_list va_list;

int printf(const char *, ...);
int vprintf(const char *, va_list ap);

void vprint(int cnt, va_list ap)
{
    printf("print(%d, ", cnt);
    while (cnt--)
        printf("%d, ", __builtin_va_arg(ap, int));
    printf(")\n");
}

void print(int cnt, ...)
{
    va_list ap;
    __builtin_va_start(ap, cnt);
    vprint(cnt, ap);
    __builtin_va_end(ap);
}

void myprintf(const char *fmt, ...)
{
    va_list ap;
    __builtin_va_start(ap, fmt);
    vprintf(fmt, ap);
    __builtin_va_end(ap);
}

int main(void)
{
    print(0);
    print(1, 0);
    print(2, 0, 1);
    print(3, 0, 1, 2);
    print(4, 0, 1, 2, 3);
    print(5, 0, 1, 2, 3, 4);
    print(6, 0, 1, 2, 3, 4, 5);
    print(7, 0, 1, 2, 3, 4, 5, 6);
    print(8, 0, 1, 2, 3, 4, 5, 6, 7);
    print(9, 0, 1, 2, 3, 4, 5, 6, 7, 8);
    print(10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    print(11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    print(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
    print(13, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    print(14, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    print(15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
    print(16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    print(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    print(18, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
    print(19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18);
    myprintf("%d %d %d %d %d %d %d %d %d %d\n", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
}
