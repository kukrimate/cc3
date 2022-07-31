// SPDX-License-Identifier: GPL-2.0-only

#ifndef _LIB_STRING_H
#define _LIB_STRING_H

typedef struct {
    size_t length;
    size_t capacity;
    char *data;
} string_t;

void string_init(string_t *self);
void string_free(string_t *self);
void string_clear(string_t *self);
void string_reserve(string_t *self, size_t new_capacity);
void string_push(string_t *self, char c);
void string_vprintf(string_t *self, const char *fmt, va_list ap);
void string_printf(string_t *self, const char *fmt, ...);

#endif
