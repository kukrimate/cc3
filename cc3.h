#ifndef CC3_H
#define CC3_H

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Enforce unreachability **/
#define ASSERT_NOT_REACHED() assert(0)

/** Exit with an error message **/
__attribute__((noreturn)) void err(const char *fmt, ...);

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
void string_printf(string_t *self, const char *fmt, ...);

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

struct tk {
    int type;
    size_t line, col;
    string_t spelling;
};

static inline const char *tk_str(tk_t *tk)
{
    return tk->spelling.data;
}

/** Lexer **/

#define LEX_BUF_SZ 8192
#define LEX_TK_CNT 2

typedef struct lexer lexer_t;

struct lexer {
    // Input file
    FILE *fp;

    // Input buffer (not really a FIFO, it's just cheaper to shift
    //               the few remaining chars up after a lookahead)
    char buf[LEX_BUF_SZ], *cur, *end;

    // Current position in the input
    size_t line, col;

    // Token buffer (FIFO)
    tk_t tk_buf[LEX_TK_CNT];
    int tk_pos, tk_cnt;

    // Current token
    tk_t *tk;
};

void lex_init(lexer_t *self, FILE *fp);
void lex_free(lexer_t *self);
tk_t *lex_tok(lexer_t *self, int i);
void lex_adv(lexer_t *self);

/** Types **/

enum {
    TY_VOID,
    TY_CHAR,
    TY_SCHAR,
    TY_UCHAR,
    TY_SHORT,
    TY_USHORT,
    TY_INT,
    TY_UINT,
    TY_LONG,
    TY_ULONG,
    TY_LLONG,
    TY_ULLONG,
    TY_FLOAT,
    TY_DOUBLE,
    TY_LDOUBLE,
    TY_BOOL,
    TY_STRUCT,
    TY_POINTER,
    TY_ARRAY,
    TY_FUNCTION,
};

typedef struct ty ty_t;

struct ty {
    ty_t *next;

    int rc;

    int kind;

    union {
        struct {
            ty_t *base_ty;
        } pointer;

        struct {
            ty_t *elem_ty;
            int cnt;
        } array;

        struct {
            ty_t *ret_ty;
            ty_t *param_tys;
            bool var;
        } function;
    };
};

ty_t *make_ty(int kind);
ty_t *make_pointer(ty_t *base_ty);
ty_t *make_array(ty_t *elem_ty, int cnt);
ty_t *make_param(ty_t *ty);
ty_t *make_function(ty_t *ret_ty, ty_t *param_tys, bool var);

ty_t *clone_ty(ty_t *ty);
void free_ty(ty_t *ty);

void print_ty(ty_t *ty);

/** Symbol table **/

typedef struct sym sym_t;

struct sym {
    sym_t *next;

    int sc;
    ty_t *ty;
    char *name;
};

typedef struct scope scope_t;

struct scope {
    scope_t *parent;
    sym_t *syms;
};

/** Semantic actions **/

typedef struct sema sema_t;

struct sema {
    scope_t *scope;
};

void sema_enter(sema_t *self);
void sema_exit(sema_t *self);
void sema_declare(sema_t *self, int sc, struct ty *ty, char *name);
sym_t *sema_lookup(sema_t *self, const char *name);
struct ty *sema_findtypedef(sema_t *self, const char *name);

/** Parser **/

typedef struct cc3 cc3_t;

struct cc3 {
    lexer_t lexer;
    sema_t sema;
};

void parse(cc3_t *self);

#endif
