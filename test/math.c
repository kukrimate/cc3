//
// Test for floating point types
//

int printf(const char *, ...);

int main(void)
{
    float f = 1.0f;
    double d = 1.0;
    long double ld = 1.0l;

    // NOTE: because default argument promotions apply to variadic arguments,
    // the float will get promoted to a double
    printf("f = %f\td = %f\tld = %Lf\n", f, d, ld);
}
