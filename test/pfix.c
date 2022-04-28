int printf(const char *, ...);

int main(void)
{
    for (int i = 0; i < 5; )    // 0 1 2 3 4
        printf("%d\n", i++);
    for (int i = 5; i > 0; )    // 5 4 3 2 1
        printf("%d\n", i--);
}
