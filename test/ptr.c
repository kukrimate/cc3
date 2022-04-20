int printf(const char *, ...);
int putchar(int);

int main(void)
{
    printf("This seems to work %d :)\n", 99);

    const char *abc = "abcdefghijklmnopqrstuvwxyz\n";
    for (; *abc; ++abc)
        putchar(*abc);

    int foo[5] = { 0, 1, 2, 3, 4 };
    
    for (int i = 0; i < 5; ++i)
        printf("foo[%d] = %d\n", i, foo[i]);

    int bar[5] = {0};
    for (int i = 0; i < 5; ++i)
        printf("bar[%d] = %d\n", i, bar[i]);
}
