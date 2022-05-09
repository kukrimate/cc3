/* Stack arguments */

int printf(const char *, ...);

// a-f go into RDI, RSI, RDX, RCX, R8, R9, rest goes on the stack
int lot_of_args(int a, int b, int c, int d, int e, int f, char ch, int i, long l)
{
    printf("ch = %c\n", ch);
    printf("i = %d\n", i);
    printf("l = %ld\n", l);
}

int main(void)
{
    // Can we pass lots of args to our own function?
    lot_of_args(0, 1, 2, 3, 4, 5, 'A', -2147483647, -9223372036854775807);

    // Can we pass lots of args to libc?
    printf("%d %d %d %d %d %d %c %d %ld\n",
        0, 1, 2, 3, 4, 5, 'A', -2147483647, -9223372036854775807);
}
