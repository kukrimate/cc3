// SPDX-License-Identifier: GPL-2.0-only

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

#ifdef MAP_USE_FNV1A

// FNV-1a by Glenn Fowler, et al

static uint32_t hash_func(const char *str)
{
    uint32_t hash = 0x811c9dc5;
    while (*s) {
        hash ^= *str++;
        hash *= 0x1000193;
    }
    return hash;
}

#else

// DJB2 by Daniel J. Bernstein

static uint32_t hash_func(const char *str)
{
    uint32_t hash = 5381;
    while (*str)
        hash = (hash << 5) + hash + *str++;
    return hash;
}

#endif

// Round up to the nearest power of two

static uint32_t round_pow2(uint32_t size)
{
    uint32_t newsize = 1;
    while (newsize < size)
        newsize <<= 1;
    return newsize;
}

// Calculate modulo a power of 2

#define MOD_POW2(val, mod) (val) & ((mod) - 1)

void map_init(map_t *self)
{
    self->count = 0;
    self->load = 0;
    self->capacity = 8;
    self->arr = calloc(self->capacity, sizeof *self->arr);
    if (!self->arr) abort();
}

void map_free(map_t *self)
{
    free(self->arr);
}

void map_clear(map_t *self)
{
    self->count = 0;
    self->load = 0;
    for (uint32_t i = 0; i < self->capacity; ++i)
        self->arr[i].state = ENTRY_EMPTY;
}

// Simplified version of find_entry for rehashes, where:
//  a) Each key only exists once in the table
//  b) And there can be no deleted entries
static entry_t *find_empty(map_t *self, uint32_t hash)
{
    uint32_t i = MOD_POW2(hash, self->capacity);
    while (self->arr[i].state != ENTRY_EMPTY)
        i = MOD_POW2(i + 1, self->capacity);
    return self->arr + i;
}

static void map_rehash(map_t *self)
{
    // Keep a copy of the old table for iteration
    uint32_t oldsize = self->capacity;
    entry_t *oldarr = self->arr;

    // The new table's load becomes count as deleted entries are eliminated
    self->load = self->count;
    // The new table size is chosen such that the initial load of the new
    // table will be at most 33% while keeping the table size a power of 2
    self->capacity = round_pow2(self->count * 3);
    self->arr = calloc(self->capacity, sizeof *self->arr);
    if (!self->arr) abort();

    // Copy all active entries from the old array
    for (uint32_t i = 0; i < oldsize; ++i)
        if (oldarr[i].state == ENTRY_ACTIVE)
            *find_empty(self, oldarr[i].hash) = oldarr[i];

    // Free the old array
    free(oldarr);
}

// Find which entry can a specified key be inserted into
static entry_t *find_entry(map_t *self, uint32_t hash, const char *key)
{
    // Re-hash when the load factor is >66%
    if (self->load > self->capacity * 2 / 3)
        map_rehash(self);

    uint32_t i = MOD_POW2(hash, self->capacity);
    int32_t d = -1;

    for (;;) {
        switch (self->arr[i].state) {
        case ENTRY_EMPTY:
            // The end of the probe chain was reached. We either return the
            // saved dummy entry for opportunistic re-filling, or if none
            // were found we return this empty entry at the end of the chain
            if (d >= 0)
                return self->arr + d;
            else
                return self->arr + i;

        case ENTRY_ACTIVE:
            // If the key of an active entry matches our search target,
            // we are done searching and return the current entry
            if (self->arr[i].hash == hash && !strcmp(self->arr[i].key, key))
                return self->arr + i;

            break;

        case ENTRY_DELETED:
            // Save the index of the first deleted entry encountered, this way
            // we can opportunistically re-fill deleted entries in our probe
            // chains instead of having to extend them
            if (d < 0)
                d = i;
        }

        // Find the next entry in the probe chain using linear probing
        i = MOD_POW2(i + 1, self->capacity);
    }
}

entry_t *map_find(map_t *self, const char *key)
{
    entry_t *entry = find_entry(self, hash_func(key), key);

    if (entry->state == ENTRY_ACTIVE)
        return entry;
    else
        return NULL;
}

entry_t *map_find_or_insert(map_t *self, const char *key, bool *found)
{
    uint32_t hash = hash_func(key);
    entry_t *entry = find_entry(self, hash, key);

    switch (entry->state) {
    case ENTRY_EMPTY:
        // Using empty entry: both load and count increases
        ++self->count;
        ++self->load;
        break;
    case ENTRY_ACTIVE:
        // Return maching entry
        *found = true;
        return entry;
    case ENTRY_DELETED:
        // Re-using deleted entry: count increases, load stays the same
        ++self->count;
        break;
    }

    *found = false;
    entry->state = ENTRY_ACTIVE;
    entry->hash = hash;
    entry->key = key;
    return entry;
}


bool map_delete(map_t *self, const char *key)
{
    entry_t *entry = find_entry(self, hash_func(key), key);

    if (entry->state == ENTRY_ACTIVE) {
        // Deleting active entry: count decreases, load stays the same
        --self->count;
        entry->state = ENTRY_DELETED;
        return true;
    } else {
        return false;
    }
}
