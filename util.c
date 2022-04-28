#include "cc3.h"

int align(int val, int bound)
{
    return (val + bound - 1) / bound * bound;
}

static void vprintln(const char *fmt, va_list ap)
{
    for (;;) {
        char ch = *fmt++;

        if (!ch)        // End of string
            break;

        if (ch == '%')  // Format specifier
            switch ((ch = *fmt++)) {
            /** Proxy to printf **/

            case '%':
                putchar('%');
                break;

            case 's':   // C string
                printf("%s", va_arg(ap, const char *));
                break;

            case 'd':   // Decimal
                printf("%d", va_arg(ap, int));
                break;

            case 'x':   // Hex
                printf("%x", va_arg(ap, int));
                break;

            case 'p':
                printf("%p", va_arg(ap, void *));
                break;

            /** Custom types **/

            case 'S':   // string_t
                printf("%s", va_arg(ap, string_t *)->data);
                break;

            case 't':   // tk_t
                printf("%s", tk_str(va_arg(ap, tk_t *)));
                break;

            case 'T':   // ty_t
                print_ty(va_arg(ap, ty_t *));
                break;

            default:    // Invalid
                abort();
            }
        else            // Normal character
            putchar(ch);
    }

    putchar('\n');
}

void println(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintln(fmt, ap);
    va_end(ap);
}

__attribute__((noreturn)) void err(const char *fmt, ...)
{
    printf("Error: ");
    va_list ap;
    va_start(ap, fmt);
    vprintln(fmt, ap);
    va_end(ap);
    abort();
}

void string_init(string_t *self)
{
    // Initial capacity is 8 chars plus the NUL terminator
    self->length = 0;
    self->capacity = 8;
    self->data = malloc(self->capacity + 1);

    // We cannot do much if allocation fails
    if (!self->data) abort();

    // Add NUL terminator
    self->data[0] = '\0';
}

void string_free(string_t *self)
{
    free(self->data);
}

void string_clear(string_t *self)
{
    // First set the length to zero
    self->length = 0;

    // Then make sure there is a NUL at the start
    self->data[0] = '\0';
}

void string_reserve(string_t *self, size_t new_capacity)
{
    assert(new_capacity > self->capacity);

    // Resize buffer to the new capacity
    self->capacity = new_capacity;
    self->data = realloc(self->data, self->capacity + 1);

    // Once again we cannot do anything on allocation failure
    if (!self->data) abort();
}

void string_push(string_t *self, char c)
{
    // Make sure there is room
    if (++self->length > self->capacity)
        string_reserve(self, self->length * 2);

    // Append character at the end
    self->data[self->length - 1] = c;

    // Ensure the new string is still NUL terminated
    self->data[self->length] = 0;
}

void string_vprintf(string_t *self, const char *fmt, va_list ap)
{
    char buf[4096];
    size_t length;

    // Produce formatted string into a local buffer
    length = vsnprintf(buf, sizeof buf, fmt, ap);

    // FIXME: we should probably dynamically resize this buffer
    assert(length < sizeof buf);

    // Then make sure it fits into the heap buffer
    size_t end = self->length;
    self->length += length;
    if (self->length > self->capacity)
        string_reserve(self, self->length * 2);

    // And finally copy it
    strcpy(self->data + end, buf);
}

void string_printf(string_t *self, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    string_vprintf(self, fmt, ap);
    va_end(ap);
}
