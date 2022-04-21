#ifndef CC3_H
#define CC3_H

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/** Enforce unreachability **/
#define ASSERT_NOT_REACHED() assert(0)

/** Align (up) val to the nearest multiple of bound **/
int align(int val, int bound);

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
void string_vprintf(string_t *self, const char *fmt, va_list ap);
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
typedef unsigned long long val_t;

struct tk {
    int type;
    // Location in the source file
    size_t line, col;
    // Spelling in the source file
    string_t spelling;
    // Semantic value (based on type)
    val_t val;      // TK_CONSTANT
    string_t str;   // TK_STR_LIT
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

typedef struct ty ty_t;
typedef struct memb memb_t;

struct memb {
    memb_t *next;

    ty_t *ty;
    char *name;

    int offset;
};

memb_t *make_memb(ty_t *ty, char *name);
memb_t *pack_struct(memb_t *members, int *out_align, int *out_size);
memb_t *pack_union(memb_t *members, int *out_align, int *out_size);

enum {
    TAG_STRUCT,
    TAG_UNION,
    TAG_ENUM,
};

typedef struct tag tag_t;

struct tag {
    tag_t *next;

    int kind;

    char *name;         // Name (can be NULL if untagged)
    bool defined;       // Is this type actuall defined?


    memb_t *members;    // Struct/union members
    int align;          // Struct/union alignment
    int size;           // Struct/union size
};

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
    TY_TAG,
    TY_POINTER,
    TY_ARRAY,
    TY_FUNCTION,
};

struct ty {
    ty_t *next;

    int kind;

    union {
        tag_t *tag;

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
ty_t *make_ty_tag(tag_t *tag);
ty_t *make_pointer(ty_t *base_ty);
ty_t *make_array(ty_t *elem_ty, int cnt);
ty_t *make_param(ty_t *ty);
ty_t *make_function(ty_t *ret_ty, ty_t *param_tys, bool var);

ty_t *clone_ty(ty_t *ty);
void free_ty(ty_t *ty);

void print_ty(ty_t *ty);

int ty_align(ty_t *ty);
int ty_size(ty_t *ty);

/** Symbol table **/

enum {
    SYM_TYPEDEF,
    SYM_EXTERN,
    SYM_STATIC,
    SYM_LOCAL,
};

typedef struct sym sym_t;

struct sym {
    sym_t *next;

    int kind;

    ty_t *ty;
    char *name;

    bool had_def;

    int offset;
};

typedef struct scope scope_t;

struct scope {
    scope_t *parent;
    sym_t *syms;
    tag_t *tags;
};

/** Semantic actions **/

typedef struct sema sema_t;

struct sema {
    scope_t *scope;
    int offset;
};

void sema_init(sema_t *self);
void sema_free(sema_t *self);
void sema_enter(sema_t *self);
void sema_exit(sema_t *self);

// Declare a name
sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, char *name);
// Lookup a declaration in any scope
sym_t *sema_lookup(sema_t *self, const char *name);
// Lookup a name in any scope, if it is a typedef, return its type
ty_t *sema_findtypedef(sema_t *self, const char *name);

// Forward declare a tag
tag_t *sema_forward_declare_tag(sema_t *self, int kind, const char *name);
// Define a tag
tag_t *sema_define_tag(sema_t *self, int kind, const char *name);


/** Expressions **/

enum {
    // Global variable
    EXPR_GLOBAL,
    // Local variable
    EXPR_LOCAL,
    // Constant
    EXPR_CONST,
    // String literal
    EXPR_STR_LIT,

    EXPR_MEMB,  // Member access
    EXPR_CALL,  // Function call

    EXPR_REF,   // Pointer creation
    EXPR_DREF,  // Pointer indirection

    EXPR_POS,   // Unary arithmetic
    EXPR_NEG,
    EXPR_NOT,

    EXPR_MUL,   // Binary arithmetic
    EXPR_DIV,
    EXPR_MOD,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_LSH,
    EXPR_RSH,
    EXPR_AND,
    EXPR_XOR,
    EXPR_OR,

    EXPR_LT,    // Boolean expressions
    EXPR_GT,
    EXPR_LE,
    EXPR_GE,
    EXPR_EQ,
    EXPR_NE,
    EXPR_LNOT,
    EXPR_LAND,
    EXPR_LOR,

    EXPR_AS,    // Assignment
    EXPR_COND,  // Conditional
    EXPR_SEQ,   // Comma operator

    EXPR_INIT,  // Initializer list
};

typedef struct expr expr_t;

struct expr {
    expr_t *next;

    int kind;

    // Type
    ty_t *ty;

    // Value
    int offset;
    char *str;
    val_t val;

    // Arguments
    expr_t *arg1, *arg2, *arg3;
};

expr_t *make_sym(sema_t *self, const char *name);
expr_t *make_const(ty_t *ty, val_t val);
expr_t *make_str_lit(const char *str);

expr_t *make_unary(int kind, expr_t *arg1);
expr_t *make_memb_expr(expr_t *arg1, const char *name);
expr_t *make_binary(int kind, expr_t *arg1, expr_t *arg2);
expr_t *make_trinary(int kind, expr_t *arg1, expr_t *arg2, expr_t *arg3);

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
    STMT_INIT,
    STMT_EVAL,
};

typedef struct stmt stmt_t;

struct stmt {
    stmt_t *next;

    int kind;

    const char *label;          // Goto label
    int case_val;               // Case label

    struct {                    // Initializer
        ty_t *ty;
        int offset;
    } init;
    
    expr_t *arg1, *arg2, *arg3; // Expression arguments
    stmt_t *body1, *body2;      // Statement list arguments
};

stmt_t *make_stmt(int kind);

/** Code generation **/

typedef struct gen gen_t;

struct gen {
    // Size of the current frame
    int offset;
    // State of the current function
    int label_cnt;
    // Number of string literals
    int str_lit_cnt;
    // Output code and data
    string_t code;
    string_t data;
};

void gen_init(gen_t *self);
void gen_free(gen_t *self);
char *gen_str_lit(gen_t *self, tk_t *tk);
void gen_func(gen_t *self, sym_t *sym, int offset, stmt_t *body);

/** Parser **/

typedef struct cc3 cc3_t;

struct cc3 {
    lexer_t lexer;
    sema_t sema;
    gen_t gen;
};

void parse(cc3_t *self);

#endif
