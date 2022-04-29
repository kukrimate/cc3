/*
 * C initializers are genuinely making me nocturnal
 */

int printf(const char *, ...);

struct foo { int a, b; };

int main(void)
{
    int arr[] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < sizeof arr / sizeof *arr; ++i)
        printf("arr[%d] = %d\n", i, arr[i]);

    struct foo arr1[] = { 1, 2, 3, 4 };
    printf("arr1[0].a = %d\n", arr1[0].a);  // 1
    printf("arr1[1].a = %d\n", arr1[1].a);  // 3

    int arr2[10] = { 1, 2, 3 };
    for (int i = 0; i < sizeof arr2 / sizeof *arr2; ++i)
        printf("arr2[%d] = %d\n", i, arr2[i]);

    struct foo s = { 5, 6 };
    printf("s.a = %d\n", s.a);
    printf("s.b = %d\n", s.b);

    char foobar[] = "foobar";
    static char foobar2[] = "foobar";
    printf("%s %s\n", foobar, foobar2);
}
