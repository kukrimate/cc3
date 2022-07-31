// SPDX-License-Identifier: GPL-2.0-only

#ifndef _LIB_VEC_H
#define _LIB_VEC_H

#define VEC_DEF(name, val_t)                                                   \
                                                                               \
typedef struct {                                                               \
    size_t length;                                                             \
    size_t capacity;                                                           \
    val_t *data;                                                               \
} name ## _vec_t;                                                              \
                                                                               \
void name ## _vec_init(name ## _vec_t *self);                                  \
void name ## _vec_free(name ## _vec_t *self);                                  \
void name ## _vec_clear(name ## _vec_t *self);                                 \
void name ## _vec_reserve(name ## _vec_t *self, size_t new_capacity);          \
val_t *name ## _vec_push(name ## _vec_t *self);                                \

#define VEC_GEN(name, val_t)                                                   \
                                                                               \
void name ## _vec_init(name ## _vec_t *self)                                   \
{                                                                              \
    self->length = 0;                                                          \
    self->capacity = 8;                                                        \
    self->data = reallocarray(NULL, self->capacity, sizeof *self->data);       \
    if (!self->data) {                                                         \
        abort();                                                               \
    }                                                                          \
}                                                                              \
                                                                               \
void name ## _vec_free(name ## _vec_t *self)                                   \
{                                                                              \
    free(self->data);                                                          \
}                                                                              \
                                                                               \
void name ## _vec_clear(name ## _vec_t *self)                                  \
{                                                                              \
    self->length = 0;                                                          \
}                                                                              \
                                                                               \
void name ## _vec_reserve(name ## _vec_t *self, size_t new_capacity)           \
{                                                                              \
    assert(new_capacity > self->capacity);                                     \
    self->capacity = new_capacity;                                             \
    self->data = reallocarray(self->data, self->capacity, sizeof *self->data); \
    if (!self->data) {                                                         \
        abort();                                                               \
    }                                                                          \
}                                                                              \
                                                                               \
val_t *name ## _vec_push(name ## _vec_t *self)                                 \
{                                                                              \
    if (++self->length > self->capacity) {                                     \
        name ## _vec_reserve(self, self->length * 2);                          \
    }                                                                          \
    return self->data + self->length - 1;                                      \
}                                                                              \

#define VEC_AT(vec, i)                                                         \
    ({                                                                         \
        __typeof__ (i) tmp = i;                                                \
        assert(tmp < (vec)->length);                                           \
        (vec)->data + tmp;                                                     \
    })                                                                         \

#define VEC_BEGIN(vec)  ((vec)->data)
#define VEC_END(vec)    ((vec)->data + (vec)->length)

#define VEC_FOREACH(vec, cur)                                                  \
    for (__typeof__ ((vec)->data) cur = (vec)->data,                           \
            _end = cur + (vec)->length; cur < _end; ++cur)                     \

#define VEC_FOREACH_REV(vec, cur)                                              \
    for (__typeof__ ((vec)->data) cur = (vec)->data + (vec)->length - 1;       \
            cur >= (vec)->data; --cur)                                         \

#endif
