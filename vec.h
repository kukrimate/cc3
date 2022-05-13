#ifndef VEC_H
#define VEC_H

#define VEC_DEF(name, memb_ty)                                                 \
                                                                               \
typedef struct {                                                               \
    size_t length;                                                             \
    size_t capacity;                                                           \
    memb_ty *data;                                                             \
} name ## _t;                                                                  \
                                                                               \
void name ## _init(name ## _t *self);                                          \
void name ## _free(name ## _t *self);                                          \
void name ## _clear(name ## _t *self);                                         \
void name ## _reserve(name ## _t *self, size_t new_capacity);                  \
memb_ty *name ## _push(name ## _t *self);                                      \

#define VEC_GEN(name, memb_ty)                                                 \
                                                                               \
void name ## _init(name ## _t *self)                                           \
{                                                                              \
    self->length = 0;                                                          \
    self->capacity = 8;                                                        \
    self->data = reallocarray(NULL, self->capacity, sizeof *self->data);       \
    if (!self->data) abort();                                                  \
}                                                                              \
                                                                               \
void name ## _free(name ## _t *self)                                           \
{                                                                              \
    free(self->data);                                                          \
}                                                                              \
                                                                               \
void name ## _clear(name ## _t *self)                                          \
{                                                                              \
    self->length = 0;                                                          \
}                                                                              \
                                                                               \
void name ## _reserve(name ## _t *self, size_t new_capacity)                   \
{                                                                              \
    assert(new_capacity > self->capacity);                                     \
    self->capacity = new_capacity;                                             \
    self->data = reallocarray(self->data, self->capacity, sizeof *self->data); \
    if (!self->data) abort();                                                  \
}                                                                              \
                                                                               \
memb_ty *name ## _push(name ## _t *self)                                       \
{                                                                              \
    if (++self->length > self->capacity)                                       \
        name ## _reserve(self, self->length * 2);                              \
    return self->data + self->length - 1;                                      \
}                                                                              \


#define VEC_AT(vec, i)                                                         \
    ({ assert(i < (vec)->length); (vec)->data + i; })                          \

#define VEC_FOREACH(vec, cur)                                                  \
    for (__typeof__ ((vec)->data) cur = (vec)->data,                           \
            end = cur + (vec)->length; cur < end; ++cur)

#define VEC_FOREACH_REV(vec, cur)                                              \
    for (__typeof__ ((vec)->data) cur = (vec)->data + (vec)->length - 1;       \
            cur >= (vec)->data; --cur)

#endif
