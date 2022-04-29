#include "cc3.h"

/** Interface to the lexer **/

static inline tk_t *peek(cc3_t *self, int i)
{
    return lex_tok(&self->lexer, i);
}

static inline void adv(cc3_t *self)
{
    lex_adv(&self->lexer);
}

static inline tk_t *next(cc3_t *self)
{
    tk_t *tk = lex_tok(&self->lexer, 0);
    lex_adv(&self->lexer);
    return tk;
}

static inline tk_t *want(cc3_t *self, int want_type)
{
    tk_t *tk = lex_tok(&self->lexer, 0);
    if (tk->type != want_type)
        err("Unexpected token %t", tk);
    lex_adv(&self->lexer);
    return tk;
}

static inline tk_t *maybe_want(cc3_t *self, int want_type)
{
    tk_t *tk = lex_tok(&self->lexer, 0);
    if (tk->type == want_type) {
        lex_adv(&self->lexer);
        return tk;
    }
    return NULL;
}

/** Expressions **/

static expr_t *primary_expression(cc3_t *self);
static expr_t *postfix_expression(cc3_t *self);
static expr_t *unary_expression(cc3_t *self);
static expr_t *cast_expression(cc3_t *self);
static expr_t *multiplicative_expression(cc3_t *self);
static expr_t *additive_expression(cc3_t *self);
static expr_t *shift_expression(cc3_t *self);
static expr_t *relational_expression(cc3_t *self);
static expr_t *equality_expression(cc3_t *self);
static expr_t *and_expression(cc3_t *self);
static expr_t *xor_expression(cc3_t *self);
static expr_t *or_expression(cc3_t *self);
static expr_t *land_expression(cc3_t *self);
static expr_t *lor_expression(cc3_t *self);
static expr_t *conditional_expression(cc3_t *self);
static expr_t *assignment_expression(cc3_t *self);
static expr_t *expression(cc3_t *self);
static int constant_expression(cc3_t *self);

static stmt_t *read_block(cc3_t *self);

expr_t *primary_expression(cc3_t *self)
{
    tk_t *tk = next(self);
    expr_t *expr;

    switch (tk->type) {
    case TK_IDENTIFIER:
        expr = make_sym(&self->sema, tk_str(tk));
        break;
    case TK_CONSTANT:
        expr = make_const(make_ty(TY_INT), tk->val);
        break;
    case TK_STR_LIT:
        {
            // NOTE: this is a little hacky
            string_t s;
            string_init(&s);
            string_printf(&s, "%s", tk->str.data);
            while ((tk = maybe_want(self, TK_STR_LIT)))
                string_printf(&s, "%s", tk->str.data);
            expr = make_str_lit(s.data);
            string_free(&s);
            break;
        }
    case TK_LPAREN:
        if (maybe_want(self, TK_LCURLY)) {  // [GNU]: statement expressions
            stmt_t *body = read_block(self);
            want(self, TK_RPAREN);
            expr = make_stmt_expr(body);
        } else {
            expr = expression(self);
            want(self, TK_RPAREN);
        }
        break;
    default:
        err("Invalid primary expression %t", tk);
    }

    return expr;
}

expr_t *postfix_expression(cc3_t *self)
{
    expr_t *expr = primary_expression(self);

    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            expr = make_unary(EXPR_DREF,
                make_binary(EXPR_ADD, expr, expression(self)));
            want(self, TK_RSQ);
            continue;
        }
        if (maybe_want(self, TK_LPAREN)) {
            expr_t *args = NULL, **tail = &args;
            if (!maybe_want(self, TK_RPAREN)) {
                do {
                    *tail = assignment_expression(self);
                    tail = &(*tail)->next;
                } while (maybe_want(self, TK_COMMA));
                want(self, TK_RPAREN);
            }
            expr = make_binary(EXPR_CALL, expr, args);
            continue;
        }
        if (maybe_want(self, TK_DOT)) {
            const char *name = tk_str(want(self, TK_IDENTIFIER));
            expr = make_memb_expr(expr, name);
            continue;
        }
        if (maybe_want(self, TK_ARROW)) {
            const char *name = tk_str(want(self, TK_IDENTIFIER));
            expr = make_memb_expr(make_unary(EXPR_DREF, expr), name);
            continue;
        }
        if (maybe_want(self, TK_INCR)) {
            // FIXME: this isn't exactly the best way to do this
            expr = make_binary(EXPR_SUB, make_binary(EXPR_AS, expr,
                make_binary(EXPR_ADD, expr, make_const(make_ty(TY_INT), 1))),
                make_const(make_ty(TY_INT), 1));
            continue;
        }
        if (maybe_want(self, TK_DECR)) {
            expr = make_binary(EXPR_ADD, make_binary(EXPR_AS, expr,
                make_binary(EXPR_SUB, expr, make_const(make_ty(TY_INT), 1))),
                make_const(make_ty(TY_INT), 1));
            continue;
        }
        // FIXME: recognize compound literals here
        return expr;
    }
}

static bool is_declaration_specifier(cc3_t *self, tk_t *tk);
static ty_t *type_name(cc3_t *self);

expr_t *unary_expression(cc3_t *self)
{
    if (maybe_want(self, TK_INCR)) {
        expr_t *arg = unary_expression(self);
        return make_binary(EXPR_AS, arg,
            make_binary(EXPR_ADD, arg, make_const(make_ty(TY_INT), 1)));
    } else if (maybe_want(self, TK_DECR)) {
        expr_t *arg = unary_expression(self);
        return make_binary(EXPR_AS, arg,
            make_binary(EXPR_SUB, arg, make_const(make_ty(TY_INT), 1)));
    } else if (maybe_want(self, TK_AND)) {
        return make_unary(EXPR_REF, cast_expression(self));
    } else if (maybe_want(self, TK_MUL)) {
        return make_unary(EXPR_DREF, cast_expression(self));
    } else if (maybe_want(self, TK_ADD)) {
        return cast_expression(self);
    } else if (maybe_want(self, TK_SUB)) {
        return make_unary(EXPR_NEG, cast_expression(self));
    } else if (maybe_want(self, TK_NOT)) {
        return make_unary(EXPR_NOT, cast_expression(self));
    } else if (maybe_want(self, TK_LNOT)) {
        return make_unary(EXPR_LNOT, cast_expression(self));
    } else if (maybe_want(self, TK_SIZEOF)) {
        ty_t *ty;
        if (peek(self, 0)->type == TK_LPAREN
                && is_declaration_specifier(self, peek(self, 1))) {
            // sizeof '(' type-name ')'
            adv(self);
            ty = type_name(self);
            want(self, TK_RPAREN);
        } else {
            // sizeof unary-expression
            ty = unary_expression(self)->ty;
        }
        return make_const(make_ty(TY_ULONG), ty_size(ty));
    } else {
        return postfix_expression(self);
    }
}

expr_t *cast_expression(cc3_t *self)
{
    if (peek(self, 0)->type == TK_LPAREN
                && is_declaration_specifier(self, peek(self, 1))) {
        // '(' type-name ')' cast-expression
        adv(self);
        type_name(self);
        want(self, TK_RPAREN);
        return cast_expression(self); // FIXME: represent cast in the AST
    } else {
        return unary_expression(self);
    }
}

expr_t *multiplicative_expression(cc3_t *self)
{
    expr_t *lhs = cast_expression(self);
    for (;;)
        if (maybe_want(self, TK_MUL))
            lhs = make_binary(EXPR_MUL, lhs, cast_expression(self));
        else if (maybe_want(self, TK_DIV))
            lhs = make_binary(EXPR_DIV, lhs, cast_expression(self));
        else if (maybe_want(self, TK_MOD))
            lhs = make_binary(EXPR_MOD, lhs, cast_expression(self));
        else
            return lhs;
}

expr_t *additive_expression(cc3_t *self)
{
    expr_t *lhs = multiplicative_expression(self);
    for (;;)
        if (maybe_want(self, TK_ADD))
            lhs = make_binary(EXPR_ADD, lhs, multiplicative_expression(self));
        else if (maybe_want(self, TK_SUB))
            lhs = make_binary(EXPR_SUB, lhs, multiplicative_expression(self));
        else
            return lhs;
}

expr_t *shift_expression(cc3_t *self)
{
    expr_t *lhs = additive_expression(self);
    for (;;)
        if (maybe_want(self, TK_LSH))
            lhs = make_binary(EXPR_LSH, lhs, additive_expression(self));
        else if (maybe_want(self, TK_RSH))
            lhs = make_binary(EXPR_RSH, lhs, additive_expression(self));
        else
            return lhs;
}

expr_t *relational_expression(cc3_t *self)
{
    expr_t *lhs = shift_expression(self);
    for (;;)
        if (maybe_want(self, TK_LT))
            lhs = make_binary(EXPR_LT, lhs, shift_expression(self));
        else if (maybe_want(self, TK_GT))
            lhs = make_binary(EXPR_GT, lhs, shift_expression(self));
        else if (maybe_want(self, TK_LE))
            lhs = make_binary(EXPR_LE, lhs, shift_expression(self));
        else if (maybe_want(self, TK_GE))
            lhs = make_binary(EXPR_GE, lhs, shift_expression(self));
        else
            return lhs;
}

expr_t *equality_expression(cc3_t *self)
{
    expr_t *lhs = relational_expression(self);
    for (;;)
        if (maybe_want(self, TK_EQ))
            lhs = make_binary(EXPR_EQ, lhs, relational_expression(self));
        else if (maybe_want(self, TK_NE))
            lhs = make_binary(EXPR_NE, lhs, relational_expression(self));
        else
            return lhs;
}

expr_t *and_expression(cc3_t *self)
{
    expr_t *lhs = equality_expression(self);
    while (maybe_want(self, TK_AND))
        lhs = make_binary(EXPR_AND, lhs, equality_expression(self));
    return lhs;
}

expr_t *xor_expression(cc3_t *self)
{
    expr_t *lhs = and_expression(self);
    while (maybe_want(self, TK_XOR))
        lhs = make_binary(EXPR_XOR, lhs, and_expression(self));
    return lhs;
}

expr_t *or_expression(cc3_t *self)
{
    expr_t *lhs = xor_expression(self);
    while (maybe_want(self, TK_OR))
        lhs = make_binary(EXPR_OR, lhs, xor_expression(self));
    return lhs;
}

expr_t *land_expression(cc3_t *self)
{
    expr_t *lhs = or_expression(self);
    while (maybe_want(self, TK_LAND))
        lhs = make_binary(EXPR_LAND, lhs, or_expression(self));
    return lhs;
}

expr_t *lor_expression(cc3_t *self)
{
    expr_t *lhs = land_expression(self);
    while (maybe_want(self, TK_LOR))
        lhs = make_binary(EXPR_LOR, lhs, land_expression(self));
    return lhs;
}

expr_t *conditional_expression(cc3_t *self)
{
    expr_t *lhs = lor_expression(self);
    if (!maybe_want(self, TK_COND))
        return lhs;
    expr_t *mid = expression(self);
    want(self, TK_COLON);
    return make_trinary(EXPR_COND, lhs, mid, conditional_expression(self));
}

expr_t *assignment_expression(cc3_t *self)
{
    expr_t *lhs = conditional_expression(self);
    if (maybe_want(self, TK_AS))
        return make_binary(EXPR_AS, lhs, assignment_expression(self));
    else if (maybe_want(self, TK_MUL_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_MUL, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_DIV_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_DIV, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_MOD_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_MOD, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_ADD_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_ADD, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_SUB_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_SUB, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_LSH_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_LSH, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_RSH_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_RSH, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_AND_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_AND, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_XOR_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_XOR, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_OR_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_OR, lhs, assignment_expression(self)));
    else
        return lhs;
}

expr_t *expression(cc3_t *self)
{
    expr_t *lhs = assignment_expression(self);
    while (maybe_want(self, TK_COMMA))
        lhs = make_binary(EXPR_SEQ, lhs, assignment_expression(self));
    return lhs;
}

int constant_expression(cc3_t *self)
{
    expr_t *expr = conditional_expression(self);
    if (expr->kind != EXPR_CONST)
        err("Expected constant expression");
    return expr->as_const.value;
}

/** Initializers **/

#if 0
static void designation(cc3_t *self)
{
    bool match = false;

    for (;;)
        if (maybe_want(self, TK_LSQ)) {
            constant_expression(self);
            want(self, TK_RSQ);
            match = true;
        } else if (maybe_want(self, TK_DOT)) {
            want(self, TK_IDENTIFIER);
            match = true;
        } else {
            goto end;
        }
end:
    if (match)
        want(self, TK_AS);
}
#endif

// Check for the end of a comma seperated list enclosed in braces
static bool end_comma_separated(cc3_t *self)
{
    if (maybe_want(self, TK_COMMA)) {
        if (maybe_want(self, TK_RCURLY))    // Trailing comma allowed
            return true;
    } else {
        want(self, TK_RCURLY);              // Otherwise the list must end
        return true;
    }
    return false;
}

static init_t *initializer(cc3_t *self)
{
    if (maybe_want(self, TK_LCURLY)) {
        init_t *head, **tail = &head;
        do {
            *tail = initializer(self);
            tail = &(*tail)->next;
        } while (!end_comma_separated(self));
        return make_init_list(head);
    } else {
        return make_init_expr(assignment_expression(self));
    }
}

/** Declarations **/

static ty_t *declaration_specifiers(cc3_t *self, int *out_sc);
static ty_t *declarator(cc3_t *self, ty_t *ty, bool allow_abstract, char **out_name);

static memb_t *member_list(cc3_t *self)
{
    memb_t *members = NULL, **tail = &members;

    do {
        int sc;
        ty_t *base_ty = declaration_specifiers(self, &sc);
        if (!base_ty)
            err("Expected declaration instead of %t", peek(self, 0));
        if (sc != -1)
            err("Storge class not allowed in struct/union");

        if (!maybe_want(self, TK_SEMICOLON)) {
            do {
                /* FIXME: anonymous bitfield
                if (maybe_want(self, TK_COLON)) {
                    constant_expression(self);
                    continue;
                }
                */

                char *name;
                ty_t *ty = declarator(self, base_ty, false, &name);

                /* FIXME: bitfield
                if (maybe_want(self, TK_COLON))
                    constant_expression(self);
                */

                // Append member
                *tail = make_memb(ty, name);
                tail = &(*tail)->next;
            } while (maybe_want(self, TK_COMMA));   // Another declarator after ,
            want(self, TK_SEMICOLON);               // Must end with ;
        } else {
            // [GNU] An anonymous struct/union is allowed as a member
            if ((base_ty->kind != TY_STRUCT && base_ty->kind != TY_UNION)
                    || !base_ty->as_aggregate.members)
                err("Invalid anonymous member");
            *tail = make_memb(base_ty, NULL);
            tail = &(*tail)->next;
        }
    } while (!maybe_want(self, TK_RCURLY));

    return members;
}

static ty_t *struct_specifier(cc3_t *self)
{
    tk_t *tk;
    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            // Tagged struct defintion
            ty_t *ty = sema_define_tag(&self->sema, TY_STRUCT, tk_str(tk));
            define_struct(ty, member_list(self));
            return ty;
        } else {
            // Forward struct declaration
            return sema_forward_declare_tag(&self->sema, TY_STRUCT, tk_str(tk));
        }
    } else {
        // Untagged struct
        want(self, TK_LCURLY);
        ty_t *ty = make_ty(TY_STRUCT);
        define_struct(ty, member_list(self));
        return ty;
    }
}

static ty_t *union_specifier(cc3_t *self)
{
    tk_t *tk;
    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            // Tagged struct defintion
            ty_t *ty = sema_define_tag(&self->sema, TY_UNION, tk_str(tk));
            define_union(ty, member_list(self));
            return ty;
        } else {
            // Forward struct declaration
            return sema_forward_declare_tag(&self->sema, TY_UNION, tk_str(tk));
        }
    } else {
        // Untagged struct
        want(self, TK_LCURLY);
        ty_t *ty = make_ty(TY_UNION);
        define_union(ty, member_list(self));
        return ty;
    }
}

static void enumerator_list(cc3_t *self)
{
    val_t cur = 0;

    for (;; ++cur) {
        // Read enumerator name
        char *name = strdup(tk_str(want(self, TK_IDENTIFIER)));

        // Optionally there might be a value
        if (maybe_want(self, TK_AS))
            cur = constant_expression(self);

        // Declare enumeration constant
        sema_declare_enum_const(&self->sema, name, cur);

        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))    // Trailing comma allowed
                return;
        } else {
            want(self, TK_RCURLY);              // Otherwise the list must end
            return;
        }
    }
}

static void enum_specifier(cc3_t *self)
{
    if (maybe_want(self, TK_IDENTIFIER)) {
        if (maybe_want(self, TK_LCURLY)) {
            // Tagged enum defintion
            enumerator_list(self);
        } else {
            // Forward enum declaration
            ;
        }
    } else {
        // Untagged enum
        want(self, TK_LCURLY);
        enumerator_list(self);
    }
}

enum {
    TS_VOID, TS_CHAR, TS_SHORT, TS_INT, TS_LONG, TS_FLOAT, TS_DOUBLE,
    TS_SIGNED, TS_UNSIGNED, TS_BOOL, TS_COMPLEX, TS_IMAGINARY, NUM_TS
};

// One of the ugliest part of C: this hand-written nightmare of a function
// decodes the a multiset of type specifiers into an actual type
static ty_t *decode_ts(int ts[static NUM_TS])
{
    static const struct {
        int ts[NUM_TS];
        int kind;
    } maps[] = {
        //  V  C  S  I  L  F  D  S  U  B  C  I
        { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, TY_VOID      },  // void
        { { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, TY_CHAR      },  // char
        { { 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, }, TY_SCHAR     },  // signed char
        { { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, }, TY_UCHAR     },  // unsigned char
        { { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, TY_SHORT     },  // short
        { { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, }, TY_SHORT     },  // signed short
        { { 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, }, TY_SHORT     },  // short int
        { { 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, }, TY_SHORT     },  // signed short int
        { { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, }, TY_USHORT    },  // unsigned short
        { { 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, }, TY_USHORT    },  // unsigned short int
        { { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, }, TY_INT       },  // int
        { { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, }, TY_INT       },  // signed
        { { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, }, TY_INT       },  // signed int
        { { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, }, TY_UINT      },  // unsigned
        { { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, }, TY_UINT      },  // unsigned int
        { { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, }, TY_LONG      },  // long
        { { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, }, TY_LONG      },  // signed long
        { { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, }, TY_LONG      },  // long int
        { { 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, }, TY_LONG      },  // signed long int
        { { 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, }, TY_LONG      },  // unsigned long
        { { 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, }, TY_LONG      },  // unsigned long int
        { { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, }, TY_LLONG     },  // long long
        { { 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, }, TY_LLONG     },  // signed long long
        { { 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, }, TY_LLONG     },  // long long int
        { { 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 0, }, TY_LLONG     },  // signed long long int
        { { 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, }, TY_ULLONG    },  // unsigned long long
        { { 0, 0, 0, 1, 2, 0, 0, 0, 1, 0, 0, 0, }, TY_ULLONG    },  // unsigned long long int
        { { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, }, TY_FLOAT     },  // float
        { { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, }, TY_DOUBLE    },  // double
        { { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, }, TY_LDOUBLE   },  // long double
        { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, }, TY_BOOL      },  // _Bool
    };

    for (int i = 0; i < sizeof maps / sizeof *maps; ++i)
        if (memcmp(ts, maps[i].ts, sizeof maps[i].ts) == 0)
            return make_ty(maps[i].kind);

    err("Provided type specifiers do not name a valid type");
}

ty_t *declaration_specifiers(cc3_t *self, int *out_sc)
{
    bool match = false;

    // Storage class
    *out_sc = -1;

    // Type specifiers
    ty_t *ty = NULL;
    bool had_ts = false;
    int ts[NUM_TS] = {0};

    for (;;) {
        // Read token
        tk_t *tk = peek(self, 0);

        // Match token
        switch (tk->type) {

        // Storage class
        case TK_TYPEDEF:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_AUTO:
        case TK_REGISTER:
            if (*out_sc != -1)
                err("Duplicate storage class");
            *out_sc = tk->type;
            break;

        // Type qualifier
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            break;

        // Function specifier
        case TK_INLINE:
            break;

        // Type specifier
        case TK_VOID:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_VOID];
            break;
        case TK_CHAR:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_CHAR];
            break;
        case TK_SHORT:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_SHORT];
            break;
        case TK_INT:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_INT];
            break;
        case TK_LONG:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_LONG];
            break;
        case TK_FLOAT:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_FLOAT];
            break;
        case TK_DOUBLE:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_DOUBLE];
            break;
        case TK_SIGNED:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_SIGNED];
            break;
        case TK_UNSIGNED:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_UNSIGNED];
            break;
        case TK_BOOL:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_BOOL];
            break;
        case TK_COMPLEX:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_COMPLEX];
            break;
        case TK_IMAGINARY:
            if (ty) err("Invalid type specifiers");
            had_ts = true;
            ++ts[TS_IMAGINARY];
            break;
        case TK_STRUCT:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            ty = struct_specifier(self);
            goto set_match;
        case TK_UNION:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            ty = union_specifier(self);
            goto set_match;
        case TK_ENUM:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            enum_specifier(self);
            ty = make_ty(TY_INT);
            goto set_match;

        // Might be a typedef name
        case TK_IDENTIFIER:
            // If there were no type specifiers, we lookup the identifier
            // to see if it's a typedef name
            if (!ty && !had_ts)
                if ((ty = sema_findtypedef(&self->sema, tk_str(tk))))
                    break;

            // If it's not a typedef name we
            // FALLTHROUGH

        // End of declaration specifiers
        default:

            if (!match)             // No type provided
                return NULL;

            if (ty)                 // Struct, union, enum, or typedef
                return ty;

            return decode_ts(ts);   // Otherwise it's a normal multiset
        }

        // Consume token
        adv(self);
set_match:
        // Note that we've matched something
        match = true;
    }
}

static bool is_declaration_specifier(cc3_t *self, tk_t *tk)
{
    switch (tk->type) {

    // Storage class
    case TK_TYPEDEF:
    case TK_EXTERN:
    case TK_STATIC:
    case TK_AUTO:
    case TK_REGISTER:

    // Type qualifier
    case TK_CONST:
    case TK_RESTRICT:
    case TK_VOLATILE:

    // Function specifier
    case TK_INLINE:

    // Type specifier
    case TK_VOID:
    case TK_CHAR:
    case TK_SHORT:
    case TK_INT:
    case TK_LONG:
    case TK_FLOAT:
    case TK_DOUBLE:
    case TK_SIGNED:
    case TK_UNSIGNED:
    case TK_BOOL:
    case TK_COMPLEX:
    case TK_IMAGINARY:
    case TK_STRUCT:
    case TK_UNION:
    case TK_ENUM:
        return true;

    // Might be a typedef name
    case TK_IDENTIFIER:
        return sema_findtypedef(&self->sema, tk_str(tk)) != NULL;

    default:
        return false;
    }
}

static void type_qualifier_list(cc3_t *self)
{
    for (;;)
        switch (peek(self, 0)->type) {
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            adv(self);
            break;
        default:
            return;
        }
}

static ty_t *declarator_suffixes(cc3_t *self, ty_t *ty)
{
    for (;;) {
        /** Array declarator **/
        if (maybe_want(self, TK_LSQ)) {
            // Function argument declared as arrays (really pointers) can be
            // qualified like normal pointers
            // Additionally static can be provided as an optimization hint,
            // we can just ignore this for now
            type_qualifier_list(self);
            maybe_want(self, TK_STATIC);
            type_qualifier_list(self);

            int cnt = -1;
            // We only allow constant array lengths, as VLA support is left out
            if (!maybe_want(self, TK_RSQ)) {
                cnt = constant_expression(self);
                want(self, TK_RSQ);
            }

            ty = make_array(ty, cnt);
            continue;
        }

        /** Function declarator **/
        if (maybe_want(self, TK_LPAREN)) {

#if 0
            // FIXME: K&R identifier list (including an empty one)
            if (maybe_want(self, TK_RPAREN)) {
                // NOTE: K&R functions are marked varargs with no parameters
                ty = make_function(ty, NULL, true);
                continue;
            }

            if (maybe_want(self, TK_IDENTIFIER)) {
                while (maybe_want(self, TK_COMMA))
                    want(self, TK_IDENTIFIER);
                want(self, TK_RPAREN);

                ty = make_function(ty, NULL, true);
                continue;
            }
#endif
            // Prototypes have their own scope
            sema_enter(&self->sema);

            // Then we can start reading the parameters
            param_t *params = NULL, **tail = &params;
            bool var = false;

            if (peek(self, 0)->type == TK_VOID && peek(self, 1)->type == TK_RPAREN) {
                // "void" as the only unnamed parameter means no parameters
                adv(self);
                adv(self);
            } else {
                for (;;) {
                    // Read parameter type
                    int sc;
                    ty_t *ty;
                    char *name;
                    if (!(ty = declaration_specifiers(self, &sc)))
                        err("Expected declaration instead of %t", peek(self, 0));
                    ty = declarator(self, ty, true, &name);

                    // Adjust parameter type as appropriate
                    switch (ty->kind) {
                    case TY_VOID:
                        err("Parameter type cannot be void");
                    case TY_ARRAY:
                        ty->kind = TY_POINTER;
                        break;
                    case TY_FUNCTION:
                        ty = make_pointer(ty);
                        break;
                    }

                    // Declare symbol if named
                    sym_t *sym = NULL;
                    if (name)
                        sym = sema_declare(&self->sema, sc, ty, name);

                    // Append parameter to the list
                    *tail = make_param(ty, sym);
                    tail = &(*tail)->next;

                    // If there is no comma the end was reached
                    if (!maybe_want(self, TK_COMMA))
                        break;

                    // Otherwise we check for ...
                    if (maybe_want(self, TK_ELLIPSIS)) {
                        var = true;
                        break;
                    }
                }

                want(self, TK_RPAREN);
            }

            ty = make_function(ty, sema_pop(&self->sema), params, var);
            continue;
        }

        /** Not a valid suffix -> we're done **/
        return ty;
    }
}

ty_t *declarator(cc3_t *self, ty_t *ty, bool allow_abstract,
    char **out_name)
{
    // Read prefix
    while (maybe_want(self, TK_MUL)) {
        type_qualifier_list(self);
        ty = make_pointer(ty);
    }

    // FIXME: in function declarators, only recurse if the follow set is
    // *not* typedef-name ')'

    if (maybe_want(self, TK_LPAREN)) {  // '(' declarator ')'
        ty_t *dummy = calloc(1, sizeof *dummy);
        ty_t *result = declarator(self, dummy, allow_abstract, out_name);
        want(self, TK_RPAREN);

        // Read suffixes
        ty = declarator_suffixes(self, ty);

        // The middle becomes the innermost declarator
        *dummy = *ty;

        return result;
    }

    tk_t *tk;

    if (allow_abstract) {
        if ((tk = maybe_want(self, TK_IDENTIFIER)))
            *out_name = strdup(tk_str(tk));
        else
            *out_name = NULL;
    } else {
        tk = want(self, TK_IDENTIFIER);
        *out_name = strdup(tk_str(tk));
    }

    return declarator_suffixes(self, ty);
}

static ty_t *type_name(cc3_t *self)
{
    int sc;
    ty_t *ty = declaration_specifiers(self, &sc);

    if (!ty)
        err("Expected type name");
    if (sc != -1)
        err("No storage class allowed");

    char *name;
    ty = declarator(self, ty, true, &name);
    if (name)
        err("Only abstract declarators are allowed");

    return ty;
}

/** Statements **/

static inline void append_stmt(stmt_t ***tail, stmt_t *stmt)
{
    **tail = stmt;
    *tail = &(**tail)->next;
}

static bool block_scope_declaration(cc3_t *self, stmt_t ***tail)
{
    int sc;
    ty_t *base_ty = declaration_specifiers(self, &sc);

    if (!base_ty)
        return false;

    if (!maybe_want(self, TK_SEMICOLON)) {
        do {
            // Read declarator
            char *name;
            ty_t *ty = declarator(self, base_ty, false, &name);

            // Trigger semantic action
            sym_t *sym = sema_declare(&self->sema, sc, ty, name);

            if (maybe_want(self, TK_AS)) {
                // Read initializer
                init_t *init = bind_init(sym->ty, initializer(self));

                // Add local initialization "statement" if the target is local
                if (sym->kind == SYM_LOCAL) {
                    stmt_t *stmt = make_stmt(STMT_INIT);
                    stmt->as_init.sym = sym;
                    stmt->as_init.value = init;
                    append_stmt(tail, stmt);
                } else {
                    gen_static(&self->gen, sym, init);
                }
            }

            sema_declare_end(&self->sema, sym);

        } while (maybe_want(self, TK_COMMA));
        // Declarators must end with ;
        want(self, TK_SEMICOLON);
    }
    return true;
}

static void statement(cc3_t *self, stmt_t ***tail);

static inline void block_item_list(cc3_t *self, stmt_t ***tail)
{
    while (!maybe_want(self, TK_RCURLY))
        if (!block_scope_declaration(self, tail))
            statement(self, tail);
}

static inline stmt_t *read_stmt(cc3_t *self)
{
    stmt_t *head = NULL, **tail = &head;
    statement(self, &tail);
    return head;
}

void statement(cc3_t *self, stmt_t ***tail)
{
    tk_t *tk;

    // Statements may be preceeded by labels
    for (;;) {
        if ((tk = peek(self, 0))->type == TK_IDENTIFIER
                && peek(self, 1)->type == TK_COLON) {
            stmt_t *stmt = make_stmt(STMT_LABEL);
            stmt->as_label.label = strdup(tk_str(tk));
            adv(self);
            adv(self);
            append_stmt(tail, stmt);
        } else if (maybe_want(self, TK_CASE)) {
            stmt_t *stmt = make_stmt(STMT_CASE);
            stmt->as_case.begin = constant_expression(self);
            if (maybe_want(self, TK_ELLIPSIS))  // [GNU]: case ranges
                stmt->as_case.end = constant_expression(self);
            else
                stmt->as_case.end = stmt->as_case.begin;
            want(self, TK_COLON);
            append_stmt(tail, stmt);
        } else if (maybe_want(self, TK_DEFAULT)) {
            want(self, TK_COLON);
            append_stmt(tail, make_stmt(STMT_DEFAULT));
        } else {
            break;
        }
    }

    // Compound statement
    if (maybe_want(self, TK_LCURLY)) {
        sema_enter(&self->sema);
        block_item_list(self, tail);
        sema_exit(&self->sema);
        return;
    }

    // If statement
    if (maybe_want(self, TK_IF)) {
        stmt_t *stmt = make_stmt(STMT_IF);

        // Heading
        want(self, TK_LPAREN);
        stmt->as_if.cond = expression(self);
        want(self, TK_RPAREN);

        // Then
        stmt->as_if.then_body = read_stmt(self);

        // Else
        if (maybe_want(self, TK_ELSE))
            stmt->as_if.else_body = read_stmt(self);

        append_stmt(tail, stmt);
        return;
    }

    // Switch statement
    if (maybe_want(self, TK_SWITCH)) {
        stmt_t *stmt = make_stmt(STMT_SWITCH);

        // Heading
        want(self, TK_LPAREN);
        stmt->as_switch.cond = expression(self);
        want(self, TK_RPAREN);

        // Body
        stmt->as_switch.body = read_stmt(self);

        append_stmt(tail, stmt);
        return;
    }

    // While statement
    if (maybe_want(self, TK_WHILE)) {
        stmt_t *stmt = make_stmt(STMT_WHILE);

        // Heading
        want(self, TK_LPAREN);
        stmt->as_while.cond = expression(self);
        want(self, TK_RPAREN);

        // Body
        stmt->as_while.body = read_stmt(self);

        append_stmt(tail, stmt);
        return;
    }

    // Do ... while statement
    if (maybe_want(self, TK_DO)) {
        stmt_t *stmt = make_stmt(STMT_DO);

        // Body
        stmt->as_do.body = read_stmt(self);

        // Condition
        want(self, TK_WHILE);
        want(self, TK_LPAREN);
        stmt->as_do.cond = expression(self);
        want(self, TK_RPAREN);
        want(self, TK_SEMICOLON);

        append_stmt(tail, stmt);
        return;
    }

    // For statement
    if (maybe_want(self, TK_FOR)) {
        stmt_t *stmt = make_stmt(STMT_FOR);

        // Heading
        want(self, TK_LPAREN);

        // C99: for creates a scope of it's own
        sema_enter(&self->sema);

        if (!block_scope_declaration(self, tail))
            if (!maybe_want(self, TK_SEMICOLON)) {
                stmt->as_for.init = expression(self);
                want(self, TK_SEMICOLON);
            }

        if (!maybe_want(self, TK_SEMICOLON)) {
            stmt->as_for.cond = expression(self);
            want(self, TK_SEMICOLON);
        }
        if (!maybe_want(self, TK_RPAREN)) {
            stmt->as_for.incr = expression(self);
            want(self, TK_RPAREN);
        }

        // Body
        stmt->as_for.body = read_stmt(self);

        // Exit for scope
        sema_exit(&self->sema);

        append_stmt(tail, stmt);
        return;
    }

    // Jumps
    if (maybe_want(self, TK_GOTO)) {
        stmt_t *stmt = make_stmt(STMT_GOTO);
        stmt->as_goto.label = strdup(tk_str(want(self, TK_IDENTIFIER)));
        want(self, TK_SEMICOLON);
        append_stmt(tail, stmt);
        return;
    }

    if (maybe_want(self, TK_CONTINUE)) {
        want(self, TK_SEMICOLON);
        append_stmt(tail, make_stmt(STMT_CONTINUE));
        return;
    }

    if (maybe_want(self, TK_BREAK)) {
        want(self, TK_SEMICOLON);
        append_stmt(tail, make_stmt(STMT_BREAK));
        return;
    }

    if (maybe_want(self, TK_RETURN)) {
        stmt_t *stmt = make_stmt(STMT_RETURN);
        if (!maybe_want(self, TK_SEMICOLON)) {
            stmt->as_return.value = expression(self);
            want(self, TK_SEMICOLON);
        }
        append_stmt(tail, stmt);
        return;
    }

    // Expression statement
    if (!maybe_want(self, TK_SEMICOLON)) {
        stmt_t *stmt = make_stmt(STMT_EVAL);
        stmt->as_return.value = expression(self);
        want(self, TK_SEMICOLON);
        append_stmt(tail, stmt);
    }
}

static stmt_t *read_block(cc3_t *self)
{
    stmt_t *head = NULL, **tail = &head;
    block_item_list(self, &tail);
    return head;
}

void parse(cc3_t *self)
{
    int sc;
    ty_t *base_ty;

    while ((base_ty = declaration_specifiers(self, &sc))) {
        if (!maybe_want(self, TK_SEMICOLON)) {
            // Read first declarator
            char *name;
            ty_t *ty = declarator(self, base_ty, false, &name);
            // Trigger semantic action
            sym_t *sym = sema_declare(&self->sema, sc, ty, name);

            // Check for function definition
            if (maybe_want(self, TK_LCURLY)) {
                // Make sure it is really a function
                if (ty->kind != TY_FUNCTION)
                    err("Expected function type instead of %T", ty);

                // Mark the symbol as having a definition
                sym->had_def = true;

                // HACK!: zero sema's picture of the frame size before
                // entering the context of a new function
                self->sema.func_name = sym->name;
                self->sema.offset = 0;

                // Now we can allocate space in the stack frame for the
                // parameters of the current function
                for (param_t *param = ty->function.params; param; param = param->next) {
                    if (!param->sym)
                        err("Unnamed parameters are not allowed in function definitions");
                    self->sema.offset = align(self->sema.offset, ty_align(param->ty));
                    param->sym->offset = self->sema.offset;
                    self->sema.offset += ty_size(param->ty);
                }

                // Bring parameters into scope for parsing the function body
                sema_push(&self->sema, ty->function.scope);
                stmt_t *body = read_block(self);
                sema_exit(&self->sema);

                // Then we can generate the function, providing the correct
                // frame size from the hack above
                gen_func(&self->gen, sym, self->sema.offset, body);
            } else {
                // If it's not a function it might have an initializer
                if (maybe_want(self, TK_AS))
                    initializer(self);

                // And further declarations might follow
                while (maybe_want(self, TK_COMMA)) {
                    char *name;
                    ty_t *ty = declarator(self, base_ty, false, &name);

                    // Trigger semantic action
                    sema_declare(&self->sema, sc, ty, name);

                    if (maybe_want(self, TK_AS))
                        initializer(self);
                }

                // The list must end with a ;
                want(self, TK_SEMICOLON);
            }
        }
    }

    want(self, TK_EOF);
}
