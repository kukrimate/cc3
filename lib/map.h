// SPDX-License-Identifier: GPL-2.0-only

#ifndef _LIB_MAP_H
#define _LIB_MAP_H

enum {
    ENTRY_EMPTY,
    ENTRY_ACTIVE,
    ENTRY_DELETED,
};

#define MAP_DEF(name, key_t, val_t)                                            \
                                                                               \
typedef struct {                                                               \
    uint8_t state;                                                             \
    uint32_t hash;                                                             \
    key_t key;                                                                 \
    val_t value;                                                               \
} name ## _entry_t;                                                            \
                                                                               \
typedef struct {                                                               \
    uint32_t count;         /* # of active entries              */             \
    uint32_t load;          /* # of active + deleted entries    */             \
    uint32_t capacity;      /* Capacity of the array            */             \
    name ## _entry_t *arr;                                                     \
} name ## _map_t;                                                              \
                                                                               \
void name ## _map_init(name ## _map_t *self);                                  \
void name ## _map_free(name ## _map_t *self);                                  \
void name ## _map_clear(name ## _map_t *self);                                 \
name ## _entry_t *name ## _map_find(name ## _map_t *self, key_t key);          \
name ## _entry_t *name ## _map_find_or_insert(name ## _map_t *self, key_t key, \
                                                bool *found);                  \
bool name ## _map_delete(name ## _map_t *self, key_t key);                     \

#define MAP_GEN(name, key_t, key_hash, key_cmp, val_t)                         \
                                                                               \
void name ## _map_init(name ## _map_t *self)                                   \
{                                                                              \
    self->count = 0;                                                           \
    self->load = 0;                                                            \
    self->capacity = 8;                                                        \
    self->arr = calloc(self->capacity, sizeof *self->arr);                     \
    if (!self->arr) {                                                          \
        abort();                                                               \
    }                                                                          \
}                                                                              \
                                                                               \
void name ## _map_free(name ## _map_t *self)                                   \
{                                                                              \
    free(self->arr);                                                           \
}                                                                              \
                                                                               \
void name ## _map_clear(name ## _map_t *self)                                  \
{                                                                              \
    self->count = 0;                                                           \
    self->load = 0;                                                            \
    for (uint32_t i = 0; i < self->capacity; ++i) {                            \
        self->arr[i].state = ENTRY_EMPTY;                                      \
    }                                                                          \
}                                                                              \
                                                                               \
/* Simplified version of find_entry for rehashes, where:                       \
    a) Each key only exists once in the table                                  \
    b) And there can be no deleted entries */                                  \
static name ## _entry_t *name ## _map_find_empty(name ## _map_t *self,         \
                                                    uint32_t hash)             \
{                                                                              \
    uint32_t i = MOD_POW2(hash, self->capacity);                               \
    while (self->arr[i].state != ENTRY_EMPTY)                                  \
        i = MOD_POW2(i + 1, self->capacity);                                   \
    return self->arr + i;                                                      \
}                                                                              \
                                                                               \
static void name ## _map_rehash(name ## _map_t *self)                          \
{                                                                              \
    /* Keep a copy of the old table for iteration */                           \
    uint32_t oldsize = self->capacity;                                         \
    name ## _entry_t *oldarr = self->arr;                                      \
                                                                               \
    /* The new table's load becomes count as deleted entries are eliminated */ \
    self->load = self->count;                                                  \
    /* The new table size is chosen such that the initial load of the new      \
       table will be at most 33% while keeping the table size a power of 2 */  \
    self->capacity = ROUND_POW2(self->count * 3);                              \
    self->arr = calloc(self->capacity, sizeof *self->arr);                     \
    if (!self->arr) {                                                          \
        abort();                                                               \
    }                                                                          \
                                                                               \
    /* Copy all active entries from the old array */                           \
    for (uint32_t i = 0; i < oldsize; ++i) {                                   \
        if (oldarr[i].state == ENTRY_ACTIVE) {                                 \
            *name ## _map_find_empty(self, oldarr[i].hash) = oldarr[i];        \
        }                                                                      \
    }                                                                          \
                                                                               \
    /* Free the old array */                                                   \
    free(oldarr);                                                              \
}                                                                              \
                                                                               \
/* Find which entry can a specified key be inserted into */                    \
static name ## _entry_t * name ## _map_find_entry(name ## _map_t *self,        \
                                                    uint32_t hash, key_t key)  \
{                                                                              \
    /* Re-hash when the load factor is >66% */                                 \
    if (self->load > self->capacity * 2 / 3)                                   \
        name ## _map_rehash(self);                                             \
                                                                               \
    uint32_t i = MOD_POW2(hash, self->capacity);                               \
    int32_t d = -1;                                                            \
                                                                               \
    for (;;) {                                                                 \
        switch (self->arr[i].state) {                                          \
        case ENTRY_EMPTY:                                                      \
            /* The end of the probe chain was reached. We either return the    \
               saved dummy entry for opportunistic re-filling, or if none      \
               were found we return this empty entry at the end of the chain */\
            if (d >= 0) {                                                      \
                return self->arr + d;                                          \
            } else {                                                           \
                return self->arr + i;                                          \
            }                                                                  \
                                                                               \
        case ENTRY_ACTIVE:                                                     \
            /* If the key of an active entry matches our search target,        \
               we are done searching and return the current entry */           \
            if (self->arr[i].hash == hash && !key_cmp(self->arr[i].key, key)) {\
                return self->arr + i;                                          \
            }                                                                  \
                                                                               \
            break;                                                             \
                                                                               \
        case ENTRY_DELETED:                                                    \
            /* Save the index of the first deleted entry encountered, this way \
               we can opportunistically re-fill deleted entries in our probe   \
               chains instead of having to extend them */                      \
            if (d < 0) {                                                       \
                d = i;                                                         \
            }                                                                  \
        }                                                                      \
                                                                               \
        /* Find the next entry in the probe chain using linear probing */      \
        i = MOD_POW2(i + 1, self->capacity);                                   \
    }                                                                          \
}                                                                              \
                                                                               \
name ## _entry_t *name ## _map_find(name ## _map_t *self, key_t key)           \
{                                                                              \
    name ## _entry_t *entry = name ## _map_find_entry(self, key_hash(key),     \
                                                        key);                  \
                                                                               \
    if (entry->state == ENTRY_ACTIVE) {                                        \
        return entry;                                                          \
    } else {                                                                   \
        return NULL;                                                           \
    }                                                                          \
}                                                                              \
                                                                               \
name ## _entry_t *name ## _map_find_or_insert(name ## _map_t *self, key_t key, \
                                                bool *found)                   \
{                                                                              \
    uint32_t hash = key_hash(key);                                             \
    name ## _entry_t *entry = name ## _map_find_entry(self, hash, key);        \
                                                                               \
    switch (entry->state) {                                                    \
    case ENTRY_EMPTY:                                                          \
        /* Using empty entry: both load and count increases */                 \
        ++self->count;                                                         \
        ++self->load;                                                          \
        break;                                                                 \
    case ENTRY_ACTIVE:                                                         \
        /* Return maching entry */                                             \
        if (found) {                                                           \
            *found = true;                                                     \
        }                                                                      \
        return entry;                                                          \
    case ENTRY_DELETED:                                                        \
        /* Re-using deleted entry: count increases, load stays the same */     \
        ++self->count;                                                         \
        break;                                                                 \
    }                                                                          \
                                                                               \
    if (found) {                                                               \
        *found = false;                                                        \
    }                                                                          \
    entry->state = ENTRY_ACTIVE;                                               \
    entry->hash = hash;                                                        \
    entry->key = key;                                                          \
    return entry;                                                              \
}                                                                              \
                                                                               \
                                                                               \
bool name ## _map_delete(name ## _map_t *self, key_t key)                      \
{                                                                              \
    name ## _entry_t *entry = name ## _map_find_entry(self, key_hash(key),     \
                                                        key);                  \
                                                                               \
    if (entry->state == ENTRY_ACTIVE) {                                        \
        /* Deleting active entry: count decreases, load stays the same */      \
        --self->count;                                                         \
        entry->state = ENTRY_DELETED;                                          \
        return true;                                                           \
    } else {                                                                   \
        return false;                                                          \
    }                                                                          \
}                                                                              \

#define MAP_ITER(map, iter)                                                    \
    for (                                                                      \
            __typeof__(*(map)->arr) *iter = (map)->arr,                        \
                                    *end = iter + (map)->capacity;             \
            ({                                                                 \
                while (iter < end && iter->state != ENTRY_ACTIVE)              \
                    ++iter;                                                    \
                iter < end;                                                    \
            });                                                                \
            ++iter                                                             \
        )                                                                      \

#endif
