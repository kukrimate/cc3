#include "cc3.h"

int align(int val, int bound)
{
    return (val + bound - 1) / bound * bound;
}

int unescape(const char *s, const char **end)
{
    int val = *s++;

    if (val == '\\')
        switch ((val = *s++)) {
        // simple-escape-sequence
        case '\'':
        case '"':
        case '?':
        case '\\':  break;
        case 'a':   val = '\a'; break;
        case 'b':   val = '\b'; break;
        case 'f':   val = '\f'; break;
        case 'n':   val = '\n'; break;
        case 'r':   val = '\r'; break;
        case 't':   val = '\t'; break;
        case 'v':   val = '\v'; break;

        // octal-escape-sequence
        case '0' ... '7':
            val -= '0';
            for (int ch;;)
                switch ((ch = *s++)) {
                case '0' ... '7':   val = val << 3 | (ch - '0'); break;
                default:            goto done;
                }

        // hexadecimal-escape-sequence
        case 'x':
            for (int ch;;)
                switch ((ch = *s++)) {
                case '0' ... '9':   val = val << 4 | (ch - '0'); break;
                case 'a' ... 'f':   val = val << 4 | (ch - 'a' + 0xa); break;
                case 'A' ... 'F':   val = val << 4 | (ch - 'A' + 0xa); break;
                default:            goto done;
                }

        default:
            err("Invalid escape sequence %c", val);
        }

done:
    if (end) *end = s;
    return val;
}

__attribute__((noreturn)) void err(const char *fmt, ...)
{
    // Print prefix
    fputs("Error: ", stderr);

    // Format error
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    // Print newline
    fputc('\n', stderr);

    // Panic
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
