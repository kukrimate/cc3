#include "cc3.h"

int align(int val, int bound)
{
    return (val + bound - 1) / bound * bound;
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
