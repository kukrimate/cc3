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
        err("Unexpected token %s", tk_str(tk));
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
        expr = make_str_lit(tk->str.data);
        break;
    case TK_LPAREN:
        expr = expression(self);
        want(self, TK_RPAREN);
        break;
    default:
        err("Invalid primary expression %s", tk_str(tk));
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
            ASSERT_NOT_REACHED();               // FIXME
            continue;
        }
        if (maybe_want(self, TK_DECR)) {
            ASSERT_NOT_REACHED();               // FIXME
            continue;
        }
        // FIXME: recognize compound literals here
        return expr;
    }
}

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
    } else {
        return postfix_expression(self);
    }
    // FIXME: sizeof unary-expression
    //    and sizeof '(' type-name ')' here
}

expr_t *cast_expression(cc3_t *self)
{
    return unary_expression(self);
    // FIXME: recognize '(' type-name ')' here
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
    return expr->val;
}

/** Declarations **/

static ty_t *declaration_specifiers(cc3_t *self, int *out_sc);
static ty_t *declarator(cc3_t *self, ty_t *ty, bool allow_abstract, char **out_name);
static void type_qualifier_list(cc3_t *self);

static memb_t *member_list(cc3_t *self)
{
    memb_t *members = NULL, **tail = &members;

    do {
        int sc;
        ty_t *base_ty = declaration_specifiers(self, &sc);
        if (!base_ty)
            err("Expected declaration in struct/union");
        if (sc != -1)
            err("Storge class not allowed in struct/union");

        do {
            /* FIXME: anonymous bitfield
            if (maybe_want(self, TK_COLON)) {
                constant_expression(self);
                continue;
            }
            */

            char *name;
            ty_t *ty = declarator(self, clone_ty(base_ty), false, &name);
            *tail = make_memb(ty, name);

            /* FIXME: bitfield
            if (maybe_want(self, TK_COLON))
                constant_expression(self);
            */

            tail = &(*tail)->next;              // Advance member list

        } while (maybe_want(self, TK_COMMA));   // Another declarator after ,

        want(self, TK_SEMICOLON);               // Must end with ;
        free_ty(base_ty);                       // Cloned for each declaration

    } while (!maybe_want(self, TK_RCURLY));

    return members;
}

static tag_t *struct_specifier(cc3_t *self)
{
    tk_t *tk;
    tag_t *tag;

    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            // Tagged struct defintion
            tag = sema_define_tag(&self->sema, TAG_STRUCT, tk_str(tk));
            tag->members = pack_struct(member_list(self), &tag->align, &tag->size);
        } else {
            // Forward struct declaration
            tag = sema_forward_declare_tag(&self->sema, TAG_STRUCT, tk_str(tk));
        }
    } else {
        // Untagged struct
        want(self, TK_LCURLY);
        tag = sema_define_tag(&self->sema, TAG_STRUCT, NULL);
        tag->members = pack_struct(member_list(self), &tag->align, &tag->size);
    }

    return tag;
}

static tag_t *union_specifier(cc3_t *self)
{
    tk_t *tk;
    tag_t *tag;

    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            // Tagged struct defintion
            tag = sema_define_tag(&self->sema, TAG_UNION, tk_str(tk));
            tag->members = pack_union(member_list(self), &tag->align, &tag->size);
        } else {
            // Forward struct declaration
            tag = sema_forward_declare_tag(&self->sema, TAG_UNION, tk_str(tk));
        }
    } else {
        // Untagged struct
        want(self, TK_LCURLY);
        tag = sema_define_tag(&self->sema, TAG_UNION, NULL);
        tag->members = pack_union(member_list(self), &tag->align, &tag->size);
    }

    return tag;
}

static void enumerator_list(cc3_t *self)
{
    for (;;) {
        // Read enumerator name
        want(self, TK_IDENTIFIER);

        // Optionally there might be a value
        if (maybe_want(self, TK_AS))
            constant_expression(self);

        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))    // Trailing comma allowed
                return;
        } else {
            want(self, TK_RCURLY);              // Otherwise the list must end
            return;
        }
    }
}

static tag_t *enum_specifier(cc3_t *self)
{
    tk_t *tk;
    tag_t *tag;

    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            // Tagged enum defintion
            sema_define_tag(&self->sema, TAG_ENUM, tk_str(tk));
            enumerator_list(self);
        } else {
            // Forward enum declaration
            sema_forward_declare_tag(&self->sema, TAG_ENUM, tk_str(tk));
        }
    } else {
        // Untagged enum
        want(self, TK_LCURLY);
        tag = sema_define_tag(&self->sema, TAG_ENUM, NULL);
        enumerator_list(self);
    }

    return tag;
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
        // void
        { { [TS_VOID] = 1 }, TY_VOID },

        // char
        { { [TS_CHAR] = 1 }, TY_CHAR },

        // signed char
        { { [TS_SIGNED] = 1, [TS_CHAR] = 1 }, TY_SCHAR },

        // unsigned char
        { { [TS_UNSIGNED] = 1, [TS_CHAR] = 1 }, TY_UCHAR },

        // short
        { { [TS_SHORT] = 1 }, TY_SHORT },
        { { [TS_SIGNED] = 1, [TS_SHORT] = 1 }, TY_SHORT },
        { { [TS_SHORT] = 1, [TS_INT] = 1 }, TY_SHORT },
        { { [TS_SIGNED] = 1, [TS_SHORT] = 1, [TS_INT] = 1 }, TY_SHORT },

        // unsigned short
        { { [TS_UNSIGNED] = 1, [TS_SHORT] = 1 }, TY_USHORT },
        { { [TS_UNSIGNED] = 1, [TS_SHORT] = 1, [TS_INT] = 1 }, TY_USHORT },

        // int
        { { [TS_INT] = 1 }, TY_INT },
        { { [TS_SIGNED] = 1 }, TY_INT },
        { { [TS_SIGNED] = 1, [TS_INT] = 1 }, TY_INT },

        // unsinged int
        { { [TS_UNSIGNED] = 1 }, TY_UINT },
        { { [TS_UNSIGNED] = 1, [TS_INT] = 1 }, TY_UINT },

        // long
        { { [TS_LONG] = 1 }, TY_LONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 1 }, TY_LONG },
        { { [TS_LONG] = 1, [TS_INT] = 1 }, TY_LONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 1, [TS_INT] = 1 }, TY_LONG },

        // unsinged long
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 1 }, TY_ULONG },
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 1, [TS_INT] = 1 }, TY_ULONG },

        // long long
        { { [TS_LONG] = 2 }, TY_LLONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 2 }, TY_LLONG },
        { { [TS_LONG] = 2, [TS_INT] = 1 }, TY_LLONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 2, [TS_INT] = 1 }, TY_LLONG },

        // unsinged long long
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 2 }, TY_ULLONG },
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 2, [TS_INT] = 1 }, TY_ULLONG },

        // float
        { { [TS_FLOAT] = 1 }, TY_FLOAT },

        // double
        { { [TS_DOUBLE] = 1 }, TY_DOUBLE },

        // long double
        { { [TS_LONG] = 1, [TS_DOUBLE] = 1 }, TY_LDOUBLE },

        // _Bool
        { { [TS_BOOL] = 1 }, TY_BOOL },
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
            ty = make_ty_tag(struct_specifier(self));
            goto set_match;
        case TK_UNION:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            ty = make_ty_tag(union_specifier(self));
            goto set_match;
        case TK_ENUM:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            ty = make_ty_tag(enum_specifier(self));
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
            // K&R identifier list (including an empty one)
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

            // "void" as the only unnamed parameter means no parameters
            if (peek(self, 0)->type == TK_VOID && peek(self, 1)->type == TK_RPAREN) {
                adv(self);
                adv(self);

                ty = make_function(ty, NULL, false);
                continue;
            }

            // Or otherwise a prototype must follow
            sema_enter(&self->sema);

            ty_t *param_tys = NULL, **tail = &param_tys;
            bool var = false;

            for (;;) {
                int sc;
                if (!(*tail = declaration_specifiers(self, &sc)))
                    break;

                char *name;
                *tail = make_param(declarator(self, *tail, true, &name));

                if (name)
                    sema_declare(&self->sema, sc, clone_ty(*tail), name);

                // If there is no comma the end was reached
                if (!maybe_want(self, TK_COMMA))
                    break;

                // Otherwise we check for ...
                if (maybe_want(self, TK_ELLIPSIS)) {
                    var = true;
                    break;
                }

                tail = &(*tail)->next;
            }

            want(self, TK_RPAREN);
            sema_exit(&self->sema);

            ty = make_function(ty, param_tys, var);
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
        free(ty);

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

void type_qualifier_list(cc3_t *self)
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

static expr_t *initializer_list(cc3_t *self);

static expr_t *initializer(cc3_t *self)
{
    // FIXME: implement these
    // designation(self);

    if (maybe_want(self, TK_LCURLY))
        return initializer_list(self);
    else
        return assignment_expression(self);
}

static expr_t *initializer_list(cc3_t *self)
{
    expr_t *head = NULL, **tail = &head;
    for (;;) {
        // Append initializer to the list
        *tail = initializer(self);
        tail = &(*tail)->next;
        // Check for the end of the list
        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))    // Trailing comma allowed
                break;
        } else {
            want(self, TK_RCURLY);              // Otherwise the list must end
            break;
        }
    }
    return make_unary(EXPR_INIT, head);
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
            ty_t *ty = declarator(self, clone_ty(base_ty), false, &name);

            // Trigger semantic action
            sym_t *sym = sema_declare(&self->sema, sc, ty, name);

            if (maybe_want(self, TK_AS)) {
                // Read initializer
                expr_t *expr = initializer(self);

                // Add local initialization "statement" if the target is local
                if (sym->offset != -1) {
                    stmt_t *stmt = make_stmt(STMT_INIT);
                    stmt->init.ty = clone_ty(sym->ty);
                    stmt->init.offset = sym->offset;
                    stmt->arg1 = expr;
                    append_stmt(tail, stmt);
                } else {
                    // FIXME: add static data
                    ASSERT_NOT_REACHED();
                }
            }
        } while (maybe_want(self, TK_COMMA));
        // Declarators must end with ;
        want(self, TK_SEMICOLON);
    }

    free_ty(base_ty);

    return true;
}

static void statement(cc3_t *self, stmt_t ***tail);

static inline void block_item_list(cc3_t *self, stmt_t ***tail)
{
    sema_enter(&self->sema);
    while (!maybe_want(self, TK_RCURLY))
        if (!block_scope_declaration(self, tail))
            statement(self, tail);
    sema_exit(&self->sema);
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
    if ((tk = peek(self, 0))->type == TK_IDENTIFIER
            && peek(self, 1)->type == TK_COLON) {
        stmt_t *stmt = make_stmt(STMT_LABEL);
        stmt->label = strdup(tk_str(tk));
        adv(self);
        adv(self);
        append_stmt(tail, stmt);
    } else if (maybe_want(self, TK_CASE)) {
        stmt_t *stmt = make_stmt(STMT_LABEL);
        stmt->case_val = constant_expression(self);
        want(self, TK_COLON);
        append_stmt(tail, stmt);
    } else if (maybe_want(self, TK_DEFAULT)) {
        want(self, TK_COLON);
        append_stmt(tail, make_stmt(STMT_DEFAULT));
    }

    // Compound statement
    if (maybe_want(self, TK_LCURLY)) {
        block_item_list(self, tail);
        return;
    }

    // If statement
    if (maybe_want(self, TK_IF)) {
        stmt_t *stmt = make_stmt(STMT_IF);

        // Heading
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
        want(self, TK_RPAREN);

        // Then
        stmt->body1 = read_stmt(self);

        // Else
        if (maybe_want(self, TK_ELSE))
            stmt->body2 = read_stmt(self);

        append_stmt(tail, stmt);
        return;
    }

    // Switch statement
    if (maybe_want(self, TK_SWITCH)) {
        stmt_t *stmt = make_stmt(STMT_SWITCH);

        // Heading
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
        want(self, TK_RPAREN);

        // Body
        stmt->body1 = read_stmt(self);

        append_stmt(tail, stmt);
        return;
    }

    // While statement
    if (maybe_want(self, TK_WHILE)) {
        stmt_t *stmt = make_stmt(STMT_WHILE);

        // Heading
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
        want(self, TK_RPAREN);

        // Body
        stmt->body1 = read_stmt(self);

        append_stmt(tail, stmt);
        return;
    }

    // Do ... while statement
    if (maybe_want(self, TK_DO)) {
        stmt_t *stmt = make_stmt(STMT_DO);

        // Body
        stmt->body1 = read_stmt(self);

        // Condition
        want(self, TK_WHILE);
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
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
                stmt->arg1 = expression(self);
                want(self, TK_SEMICOLON);
            }

        if (!maybe_want(self, TK_SEMICOLON)) {
            stmt->arg2 = expression(self);
            want(self, TK_SEMICOLON);
        }
        if (!maybe_want(self, TK_RPAREN)) {
            stmt->arg3 = expression(self);
            want(self, TK_RPAREN);
        }

        // Body
        stmt->body1 = read_stmt(self);

        // Exit for scope
        sema_exit(&self->sema);

        append_stmt(tail, stmt);
        return;
    }

    // Jumps
    if (maybe_want(self, TK_GOTO)) {
        stmt_t *stmt = make_stmt(STMT_GOTO);
        want(self, TK_IDENTIFIER);
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
            stmt->arg1 = expression(self);
            want(self, TK_SEMICOLON);
        }
        append_stmt(tail, stmt);
        return;
    }

    // Expression statement
    if (!maybe_want(self, TK_SEMICOLON)) {
        stmt_t *stmt = make_stmt(STMT_EVAL);
        stmt->arg1 = expression(self);
        want(self, TK_SEMICOLON);
        append_stmt(tail, stmt);
    }
}

static inline stmt_t *read_block(cc3_t *self)
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
            ty_t *ty = declarator(self, clone_ty(base_ty), false, &name);

            // Trigger semantic action
            sym_t *sym = sema_declare(&self->sema, sc, ty, name);

            // Check for function definition
            if (maybe_want(self, TK_LCURLY)) {
                // Mark the symbol as having a definition
                sym->had_def = true;
                // HACK!: zero sema's picture of the frame size before
                // entering the context of a new function
                self->sema.offset = 0;
                stmt_t *body = read_block(self);
                // Then we can generate the function, providing the correct
                // frame size from the hack above
                gen_func(&self->gen, sym, self->sema.offset, body);
                free_ty(base_ty);
                continue;
            }

            // If it's not a function it might have an initializer
            if (maybe_want(self, TK_AS))
                initializer(self);

            // And further declarations might follow
            while (maybe_want(self, TK_COMMA)) {
                char *name;
                ty_t *ty = declarator(self, clone_ty(base_ty), false, &name);

                // Trigger semantic action
                sema_declare(&self->sema, sc, ty, name);

                if (maybe_want(self, TK_AS))
                    initializer(self);
            }

            // The list must end with a ;
            want(self, TK_SEMICOLON);
        }

        // Free base type
        free_ty(base_ty);
    }
}
