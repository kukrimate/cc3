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

struct tk_buf {
    /** Type **/
    int type;

    /** Value **/
    int intval;
    char strval[4096];
};

const char *tk_str(int type);


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

struct member {
    struct member *next;

    struct ty *ty;
    const char *name;
};

struct ty {
    struct ty *next;

    int kind;

    union {
        struct {
            struct member *members;
        } stru;

        struct {
            struct ty *base_ty;
        } pointer;

        struct {
            struct ty *elem_ty;
            int cnt;
        } array;

        struct {
            struct ty *ret_ty;
            struct ty *param_tys;
            bool var;
        } function;
    };
};

struct ty *make_ty(int kind);
struct ty *make_pointer(struct ty *base_ty);
struct ty *make_array(struct ty *elem_ty, int cnt);
struct ty *make_function(struct ty *ret_ty, struct ty *param_tys, bool var);

void print_ty(struct ty *ty);

/** Symbol table **/

struct sym {
    struct sym *next;

    int sc;
    struct ty *ty;
    const char *name;
};

struct scope {
    struct scope *parent;
    struct sym *syms;
};

/** Compiler context **/

#define LEX_BUF_SZ 8192
#define LEX_TK_CNT 2

struct cc3 {
    // Input file
    FILE *fp;

    // Input buffer (not really a FIFO, it's just cheaper to shift
    //               the few remaining chars up after a lookahead)
    char buf[LEX_BUF_SZ], *cur, *end;

    // Token buffer (FIFO)
    struct tk_buf tk_buf[LEX_TK_CNT];
    int tk_pos, tk_cnt;

    // Symbol table
    struct scope *cur_scope;
};

void cc3_init(struct cc3 *self, FILE *fp);

/** Tokenizer **/

struct tk_buf *lex_tok(struct cc3 *self, int i);
void lex_adv(struct cc3 *self);

/** Parser **/

void parse(struct cc3 *self);

/** Semantic actions **/

void sema_enter(struct cc3 *self);
void sema_exit(struct cc3 *self);
void sema_declare(struct cc3 *self, int sc, struct ty *ty, const char *name);
struct sym *sema_lookup(struct cc3 *self, const char *name);
struct ty *sema_findtypedef(struct cc3 *self, const char *name);

#endif
