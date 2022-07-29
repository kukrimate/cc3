// SPDX-License-Identifier: GPL-2.0-only

#ifndef CC3_H
#define CC3_H

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "vec.h"

/** Size of an array in elements **/
#define ARRAY_SIZE(array) (sizeof (array) / sizeof *(array))

/** Enforce unreachability **/
#define ASSERT_NOT_REACHED() assert(0)

/** Move a value to a heap allocation **/
#define BOX(val)                                        \
    ({                                                  \
        __typeof__(val) *ptr = malloc(sizeof val);      \
        if (!ptr) abort();                              \
        *ptr = val;                                     \
        ptr;                                            \
    })

/** Align (up) val to the nearest multiple of bound **/
int align(int val, int bound);

/** Print a message **/
void println(const char *fmt, ...);

/** Exit with an error message **/
__attribute__((noreturn)) void err(const char *fmt, ...);

/** Print a debugging message **/
#ifdef NDEBUG
#define debugln(fmt, ...)
#else
#define debugln(fmt, ...) println(fmt, ## __VA_ARGS__)
#endif

/** Switch fallthrough marker **/
#ifdef __GNUC__
#define FALLTHROUGH __attribute__((fallthrough))
#else
#define FALLTHROUGH
#endif

/** Resizable string abstraction **/
typedef struct string string_t;

struct string {
    size_t length;
    size_t capacity;
    char *data;
};

void string_init(string_t *self);
void string_free(string_t *self);
void string_clear(string_t *self);
void string_reserve(string_t *self, size_t new_capacity);
void string_push(string_t *self, char c);
void string_vprintf(string_t *self, const char *fmt, va_list ap);
void string_printf(string_t *self, const char *fmt, ...);

/** Hash table **/

typedef struct ty ty_t;
typedef struct sym sym_t;

enum {
    ENTRY_EMPTY,
    ENTRY_ACTIVE,
    ENTRY_DELETED
};

typedef struct {
    uint8_t state;
    uint32_t hash;
    const char *key;

    union {
        ty_t *as_ty;
        sym_t *as_sym;
        int as_int;
    };
} entry_t;

typedef struct {
    uint32_t count;     // # of active entries
    uint32_t load;      // # of active + deleted entries
    uint32_t capacity;  // Capacity of the array
    entry_t *arr;
} map_t;

void map_init(map_t *self);
void map_free(map_t *self);
void map_clear(map_t *self);
entry_t *map_find(map_t *self, const char *key);
entry_t *map_find_or_insert(map_t *self, const char *key, bool *found);
bool map_delete(map_t *self, const char *key);

#define MAP_ITER(map, iter)                                         \
    for (entry_t *iter = (map)->arr, *end = iter + (map)->capacity; \
            ({                                                      \
                while (iter < end && iter->state != ENTRY_ACTIVE)   \
                    ++iter;                                         \
                iter < end;                                         \
            }); ++iter)

/** Tokens **/

enum {
    TK_EOF,

    TK_AUTO,
    TK_BREAK,
    TK_CASE,
    TK_CHAR,
    TK_CONST,
    TK_CONTINUE,
    TK_DEFAULT,
    TK_DO,
    TK_DOUBLE,
    TK_ELSE,
    TK_ENUM,
    TK_EXTERN,
    TK_FLOAT,
    TK_FOR,
    TK_GOTO,
    TK_IF,
    TK_INLINE,
    TK_INT,
    TK_LONG,
    TK_REGISTER,
    TK_RESTRICT,
    TK_RETURN,
    TK_SHORT,
    TK_SIGNED,
    TK_SIZEOF,
    TK_STATIC,
    TK_STRUCT,
    TK_SWITCH,
    TK_TYPEDEF,
    TK_UNION,
    TK_UNSIGNED,
    TK_VOID,
    TK_VOLATILE,
    TK_WHILE,
    TK_BOOL,
    TK_COMPLEX,
    TK_IMAGINARY,

    TK_TYPEOF,
    TK_VA_LIST,
    TK_VA_START,
    TK_VA_END,
    TK_VA_ARG,

    TK_IDENTIFIER,
    TK_CONSTANT,
    TK_STR_LIT,

    TK_LSQ,             // [
    TK_RSQ,             // ]
    TK_LPAREN,          // (
    TK_RPAREN,          // )
    TK_LCURLY,          // {
    TK_RCURLY,          // }
    TK_DOT,             // .
    TK_ARROW,           // ->
    TK_INCR,            // ++
    TK_DECR,            // --
    TK_AND,             // &
    TK_MUL,             // *
    TK_ADD,             // +
    TK_SUB,             // -
    TK_NOT,             // ~
    TK_LNOT,            // !
    TK_DIV,             // /
    TK_MOD,             // %
    TK_LSH,             // <<
    TK_RSH,             // >>
    TK_LT,              // <
    TK_GT,              // >
    TK_LE,              // <=
    TK_GE,              // >=
    TK_EQ,              // ==
    TK_NE,              // !=
    TK_XOR,             // ^
    TK_OR,              // |
    TK_LAND,            // &&
    TK_LOR,             // ||
    TK_COND,            // ?
    TK_COLON,           // :
    TK_SEMICOLON,       // ;
    TK_ELLIPSIS,        // ...
    TK_AS,              // =
    TK_MUL_AS,          // *=
    TK_DIV_AS,          // /=
    TK_MOD_AS,          // %=
    TK_ADD_AS,          // +=
    TK_SUB_AS,          // -=
    TK_LSH_AS,          // <<=
    TK_RSH_AS,          // >>=
    TK_AND_AS,          // &=
    TK_XOR_AS,          // ^=
    TK_OR_AS,           // |=
    TK_COMMA,           // ,
    TK_HASH,            // #
    TK_PASTE,           // ##

    TK_ERROR,           // Unknown character
};

typedef struct tk tk_t;
typedef unsigned long long val_t;

struct tk {
    unsigned char kind;

    // Location in the source file
    size_t line, col;

    // Spelling in the source file
    const char *spelling;

    // Semantic value (based on kind)
    union {
        val_t val;          // TK_CONSTANT
        const char *str;    // TK_STR_LIT
    };
};

static inline const char *tk_str(tk_t *tk)
{
    return tk->spelling;
}

/** Lexer **/

#define LEX_BUF_SZ 8192

typedef struct lexer lexer_t;

struct lexer {
    // Input file
    int in_fd;

    // Input buffer (not really a FIFO, it's just cheaper to shift
    //               the few remaining chars up after a lookahead)
    char buf[LEX_BUF_SZ], *cur, *end;

    // Current position in the input
    size_t line, col;

    // Token spelling buffer
    string_t spelling;

    // String literal decoding buffer
    string_t decode;
};

void lex_init(lexer_t *self, int in_fd);
void lex_free(lexer_t *self);
void lex_next(lexer_t *self, tk_t *tk);


/** Types **/

typedef struct {
    ty_t *ty;
    const char *name;
    int offset;
} memb_t;

VEC_DEF(memb_vec, memb_t)

typedef struct {
    ty_t *ty;
    sym_t *sym;
} param_t;

VEC_DEF(param_vec, param_t)

// Type kinds are a non-overlapping bitflag collection. This allows cheap
// testing for a group of types.

enum {
    TY_VOID     = (1 << 0),
    TY_BOOL     = (1 << 1),
    TY_CHAR     = (1 << 2),
    TY_SCHAR    = (1 << 3),
    TY_UCHAR    = (1 << 4),
    TY_SHORT    = (1 << 5),
    TY_USHORT   = (1 << 6),
    TY_INT      = (1 << 7),
    TY_UINT     = (1 << 8),
    TY_LONG     = (1 << 9),
    TY_ULONG    = (1 << 10),
    TY_LLONG    = (1 << 11),
    TY_ULLONG   = (1 << 12),
    TY_FLOAT    = (1 << 13),
    TY_DOUBLE   = (1 << 14),
    TY_LDOUBLE  = (1 << 15),
    TY_STRUCT   = (1 << 16),
    TY_UNION    = (1 << 17),
    TY_POINTER  = (1 << 18),
    TY_ARRAY    = (1 << 19),
    TY_FUNCTION = (1 << 20),

    // Types where the integer promotions apply
    TY_PROMOTE_MASK = TY_BOOL | TY_CHAR | TY_SCHAR
                    | TY_UCHAR | TY_SHORT | TY_USHORT,

    // Integer types
    TY_INT_MASK = TY_BOOL | TY_CHAR | TY_SCHAR | TY_UCHAR
                | TY_SHORT | TY_USHORT | TY_INT | TY_UINT
                | TY_LONG | TY_ULONG | TY_LLONG | TY_ULLONG,

    // Arithmetic type
    TY_ARITH_MASK = TY_INT_MASK | TY_FLOAT | TY_DOUBLE | TY_LDOUBLE,

    // Scalar type
    TY_SCALAR_MASK = TY_ARITH_MASK | TY_POINTER | TY_ARRAY | TY_FUNCTION,
};

typedef struct scope scope_t;

struct ty {
    int kind;

    int align;
    int size;

    union {
        struct {
            ty_t *elem_ty;
            int cnt;
        } array;

        struct {
            const char *name;
            bool had_def;
            memb_vec_t members;
        } as_aggregate;

        struct {
            ty_t *base_ty;
        } pointer;

        struct {
            ty_t *ret_ty;
            scope_t *scope;
            param_vec_t params;
            bool var;
        } function;
    };
};

extern ty_t ty_void;
extern ty_t ty_char;
extern ty_t ty_schar;
extern ty_t ty_uchar;
extern ty_t ty_short;
extern ty_t ty_ushort;
extern ty_t ty_int;
extern ty_t ty_uint;
extern ty_t ty_long;
extern ty_t ty_ulong;
extern ty_t ty_llong;
extern ty_t ty_ullong;
extern ty_t ty_float;
extern ty_t ty_double;
extern ty_t ty_ldouble;
extern ty_t ty_bool;
extern ty_t ty_va_list;

ty_t *make_ty(int kind);
ty_t *make_pointer(ty_t *base_ty);
ty_t *make_array(ty_t *elem_ty, int cnt);
ty_t *make_function(ty_t *ret_ty, scope_t *scope, param_vec_t *params, bool var);

void print_ty(ty_t *ty);

/** Symbol table **/

enum {
    SYM_TYPEDEF,
    SYM_EXTERN,
    SYM_STATIC,
    SYM_LOCAL,
    SYM_ENUM_CONST,
};

struct sym {
    // Symbol kind
    int kind;

    // Declared type and name
    ty_t *ty;
    const char *name;

    // Assembler symbol name
    const char *asm_name;

    // Offset of the storage on the stack (for SYM_LOCAL)
    int offset;
    // Constant value of the symbol (for SYM_ENUM_CONST)
    val_t val;
};

struct scope {
    scope_t *parent;
    map_t syms;
    map_t tags;
};

/** Semantic actions **/

typedef struct sema sema_t;

struct sema {
    scope_t *scope;

    // Block scope static count
    int block_static_cnt;

    // HACK: current function specific stuff
    const char *func_name;
};

void sema_init(sema_t *self);
void sema_free(sema_t *self);

void sema_enter(sema_t *self);
void sema_exit(sema_t *self);
void sema_push(sema_t *self, scope_t *scope);
scope_t *sema_pop(sema_t *self);

// Declare a name
sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, const char *name);
// Declare an enumeration constant
sym_t *sema_declare_enum_const(sema_t *self, const char *name, val_t val);
// Lookup a declaration in any scope
sym_t *sema_lookup(sema_t *self, const char *name);
// Lookup a name in any scope, if it is a typedef, return its type
ty_t *sema_findtypedef(sema_t *self, const char *name);

// Forward declare a tag
ty_t *sema_forward_declare_tag(sema_t *self, int kind, const char *name);
// Define a tag
ty_t *sema_define_tag(sema_t *self, int kind, const char *name);


/** Expressions **/

typedef struct expr expr_t;
typedef struct stmt stmt_t;

VEC_DEF(expr_vec, expr_t *)
VEC_DEF(stmt_vec, stmt_t)

enum {
    EXPR_SYM,
    EXPR_CONST,
    EXPR_STR,

    EXPR_MEMB,
    EXPR_CALL,

    EXPR_REF,
    EXPR_DREF,
    EXPR_NEG,
    EXPR_NOT,
    EXPR_LNOT,

    EXPR_CAST,

    EXPR_MUL,
    EXPR_DIV,
    EXPR_MOD,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_LSH,
    EXPR_RSH,
    EXPR_LT,
    EXPR_GT,
    EXPR_LE,
    EXPR_GE,
    EXPR_EQ,
    EXPR_NE,
    EXPR_AND,
    EXPR_XOR,
    EXPR_OR,
    EXPR_LAND,
    EXPR_LOR,
    EXPR_COND,
    EXPR_AS,
    EXPR_SEQ,

    EXPR_STMT,  // [GNU]

    EXPR_VA_START,
    EXPR_VA_END,
    EXPR_VA_ARG,
};

struct expr {
    int kind;

    ty_t *ty;

    union {
        sym_t *as_sym;

        struct {
            val_t value;
        } as_const;

        struct {
            const char *data;
        } as_str;

        struct {
            expr_t *aggr;
            const char *name;
            int offset;
        } as_memb;

        struct {
            expr_t *func;
            expr_vec_t args;
        } as_call;

        struct {
            expr_t *arg;
        } as_unary;

        struct {
            expr_t *lhs;
            expr_t *rhs;
        } as_binary;

        struct {
            expr_t *arg1;
            expr_t *arg2;
            expr_t *arg3;
        } as_trinary;

        stmt_vec_t as_stmts;
    };
};

expr_t *alloc_expr(int kind);

expr_t *make_sym_expr(sema_t *self, const char *name);
expr_t *make_const_expr(ty_t *ty, val_t value);
expr_t *make_str_expr(const char *data);

expr_t *make_memb_expr(expr_t *aggr, const char *name);
expr_t *make_call_expr(expr_t *func, expr_vec_t *args);

expr_t *make_ref_expr(expr_t *arg);
expr_t *make_dref_expr(expr_t *arg);
expr_t *make_pos_expr(expr_t *arg);
expr_t *make_neg_expr(expr_t *arg);
expr_t *make_not_expr(expr_t *arg);
expr_t *make_lnot_expr(expr_t *arg);
expr_t *make_sizeof_expr(ty_t *ty);

expr_t *make_cast_expr(ty_t *ty, expr_t *arg);

expr_t *make_mul_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_div_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_mod_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_add_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_sub_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_lsh_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_rsh_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_lt_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_gt_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_le_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_ge_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_eq_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_ne_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_and_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_xor_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_or_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_land_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_lor_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_cond_expr(expr_t *cond, expr_t *val1, expr_t *val2);
expr_t *make_as_expr(expr_t *lhs, expr_t *rhs);
expr_t *make_seq_expr(expr_t *lhs, expr_t *rhs);

expr_t *make_stmt_expr(stmt_vec_t *stmts);

void print_expr(expr_t *expr);

/** Initializer **/

enum {
    INIT_EXPR,
    INIT_LIST,
};

typedef struct init init_t;

VEC_DEF(init_vec, init_t)

struct init {
    int kind;

    union {
        expr_t *as_expr;
        init_vec_t as_list;
    };
};

void make_init_expr(init_t *out, ty_t *dest_ty, expr_t *expr);
void make_init_list(init_t *out, init_vec_t *list);

/** Statements **/

enum {
    STMT_LABEL,
    STMT_CASE,
    STMT_DEFAULT,
    STMT_IF,
    STMT_SWITCH,
    STMT_WHILE,
    STMT_DO,
    STMT_FOR,
    STMT_GOTO,
    STMT_CONTINUE,
    STMT_BREAK,
    STMT_RETURN,
    STMT_EVAL,
    STMT_DECL,
};

struct stmt {
    int kind;

    union {
        struct {
            const char *label;
        } as_label, as_goto;

        struct {
            int begin;
            int end;
        } as_case;      // [GNU]: allow case ranges as well

        struct {
            expr_t *cond;
            stmt_vec_t  then_body,
                        else_body;
        } as_if;

        struct {
            expr_t *cond;
            stmt_vec_t  body;
        } as_switch, as_while, as_do;

        struct {
            expr_t *init, *cond, *incr;
            stmt_vec_t body;
        } as_for;

        struct {
            expr_t *value;
        } as_return, as_eval;

        struct {
            sym_t *sym;
            bool has_init;
            init_t init;
        } as_decl;
    };
};

void print_stmts(stmt_vec_t *stmts, int indent);

/** Code generation **/

typedef struct {
    int label;
    int begin;
    int end;
} case_t;

VEC_DEF(case_vec, case_t)

typedef struct gen {
    /** Global state **/
    int out_fd;
    // Next unique label #
    int label_cnt;
    // String literals
    map_t lits;

    /** Function state **/

    // Current offset into the frame
    int offset;
    // Initial values for va_lists
    int gp_offset;
    int fp_offset;
    // Number of active temporaries
    int temp_cnt;

    // Jump targets
    int return_label;
    int break_label;
    int continue_label;
    int default_label;
    case_vec_t *cases;
    map_t gotos;
} gen_t;

void gen_init(gen_t *self, int out_fd);
void gen_free(gen_t *self);

void gen_static(gen_t *self, sym_t *sym, init_t *init);
void gen_func(gen_t *self, sym_t *sym, stmt_vec_t *stmts);
void gen_lits(gen_t *self);

/** Parser **/

void cc3_compile(int in_fd, int out_fd);

#endif
