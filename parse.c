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

static void primary_expression(cc3_t *self);
static void postfix_expression(cc3_t *self);
static void unary_expression(cc3_t *self);
static void cast_expression(cc3_t *self);
static void multiplicative_expression(cc3_t *self);
static void additive_expression(cc3_t *self);
static void shift_expression(cc3_t *self);
static void relational_expression(cc3_t *self);
static void equality_expression(cc3_t *self);
static void and_expression(cc3_t *self);
static void xor_expression(cc3_t *self);
static void or_expression(cc3_t *self);
static void land_expression(cc3_t *self);
static void lor_expression(cc3_t *self);
static void conditional_expression(cc3_t *self);
static void assignment_expression(cc3_t *self);
static void expression(cc3_t *self);
static void constant_expression(cc3_t *self);

void primary_expression(cc3_t *self)
{
    tk_t *tk = next(self);

    switch (tk->type) {
    case TK_IDENTIFIER:
    case TK_CONSTANT:
    case TK_STR_LIT:
        break;
    case TK_LPAREN:
        expression(self);
        want(self, TK_RPAREN);
        break;
    default:
        err("Invalid primary expression %s", tk_str(tk));
    }
}

void postfix_expression(cc3_t *self)
{
    primary_expression(self);

    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            expression(self);
            want(self, TK_RSQ);
            continue;
        }

        if (maybe_want(self, TK_LPAREN)) {
            // Empty argument list
            if (maybe_want(self, TK_RPAREN))
                continue;

            // Otherwise we have a comma seperated list of assignment-expressions
            do
                assignment_expression(self);
            while (maybe_want(self, TK_COMMA));

            want(self, TK_RPAREN);
            continue;
        }

        if (maybe_want(self, TK_DOT)) {
            want(self, TK_IDENTIFIER);
            continue;
        }

        if (maybe_want(self, TK_ARROW)) {
            want(self, TK_IDENTIFIER);
            continue;
        }

        if (maybe_want(self, TK_INCR))
            ;

        if (maybe_want(self, TK_DECR))
            ;

        break;
    }

    // FIXME: recognize compound literals here
}

void unary_expression(cc3_t *self)
{
    if (maybe_want(self, TK_INCR))
        unary_expression(self);
    else if (maybe_want(self, TK_DECR))
        unary_expression(self);
    else if (maybe_want(self, TK_AND))
        cast_expression(self);
    else if (maybe_want(self, TK_MUL))
        cast_expression(self);
    else if (maybe_want(self, TK_ADD))
        cast_expression(self);
    else if (maybe_want(self, TK_SUB))
        cast_expression(self);
    else if (maybe_want(self, TK_NOT))
        cast_expression(self);
    else if (maybe_want(self, TK_LNOT))
        cast_expression(self);
    else
        postfix_expression(self);

    // FIXME: sizeof unary-expression
    //    and sizeof '(' type-name ')' here
}

void cast_expression(cc3_t *self)
{
    unary_expression(self);
    // FIXME: recognize '(' type-name ')' here
}

void multiplicative_expression(cc3_t *self)
{
    cast_expression(self);
    for (;;)
        if (maybe_want(self, TK_MUL))
            cast_expression(self);
        else if (maybe_want(self, TK_DIV))
            cast_expression(self);
        else if (maybe_want(self, TK_MOD))
            cast_expression(self);
        else
            return;
}

void additive_expression(cc3_t *self)
{
    multiplicative_expression(self);
    for (;;)
        if (maybe_want(self, TK_ADD))
            multiplicative_expression(self);
        else if (maybe_want(self, TK_SUB))
            multiplicative_expression(self);
        else
            return;
}

void shift_expression(cc3_t *self)
{
    additive_expression(self);
    for (;;)
        if (maybe_want(self, TK_LSH))
            additive_expression(self);
        else if (maybe_want(self, TK_RSH))
            additive_expression(self);
        else
            return;
}

void relational_expression(cc3_t *self)
{
    shift_expression(self);
    for (;;)
        if (maybe_want(self, TK_LT))
            shift_expression(self);
        else if (maybe_want(self, TK_GT))
            shift_expression(self);
        else if (maybe_want(self, TK_LE))
            shift_expression(self);
        else if (maybe_want(self, TK_GE))
            shift_expression(self);
        else
            return;
}

void equality_expression(cc3_t *self)
{
    relational_expression(self);
    for (;;)
        if (maybe_want(self, TK_EQ))
            relational_expression(self);
        else if (maybe_want(self, TK_NE))
            relational_expression(self);
        else
            return;
}

void and_expression(cc3_t *self)
{
    equality_expression(self);
    while (maybe_want(self, TK_AND))
        equality_expression(self);
}

void xor_expression(cc3_t *self)
{
    and_expression(self);
    while (maybe_want(self, TK_XOR))
        and_expression(self);
}

void or_expression(cc3_t *self)
{
    xor_expression(self);
    while (maybe_want(self, TK_OR))
        xor_expression(self);
}

void land_expression(cc3_t *self)
{
    or_expression(self);
    while (maybe_want(self, TK_LAND))
        or_expression(self);
}

void lor_expression(cc3_t *self)
{
    land_expression(self);
    while (maybe_want(self, TK_LOR))
        land_expression(self);
}

void conditional_expression(cc3_t *self)
{
    lor_expression(self);

    if (!maybe_want(self, TK_COND))
        return;

    expression(self);

    want(self, TK_COLON);

    conditional_expression(self);
}

void assignment_expression(cc3_t *self)
{
    conditional_expression(self);

    if (maybe_want(self, TK_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_MUL_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_DIV_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_MOD_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_ADD_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_SUB_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_LSH_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_RSH_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_AND_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_XOR_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_OR_AS))
        assignment_expression(self);
}

void expression(cc3_t *self)
{
    assignment_expression(self);
    while (maybe_want(self, TK_COMMA))
        assignment_expression(self);
}

void constant_expression(cc3_t *self)
{
    conditional_expression(self);
}

/** Declarations **/

static ty_t *declaration_specifiers(cc3_t *self, int *out_sc);
static void struct_declaration_list(cc3_t *self);
static void enumerator_list(cc3_t *self);
static ty_t *declarator(cc3_t *self, ty_t *ty,
    bool allow_abstract, char **out_name);
static void type_qualifier_list(cc3_t *self);
static void initializer(cc3_t *self);
static void initializer_list(cc3_t *self);
static void designation(cc3_t *self);

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
        { { [TS_SIGNED] = 1, [TS_SHORT] = 1, [TY_INT] = 1 }, TY_SHORT },

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

static ty_t *declaration_specifiers(cc3_t *self, int *out_sc)
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
        case TK_UNION:
            adv(self);

            if (ty || had_ts)
                err("Invalid type specifiers");

            // Might be followed by an identifier
            if ((tk = maybe_want(self, TK_IDENTIFIER)))
                ;

            // Read member declarations if present
            if (maybe_want(self, TK_LCURLY))
                struct_declaration_list(self);

            continue;

        case TK_ENUM:
            adv(self);

            if (ty || had_ts)
                err("Invalid type specifiers");

            // Might be followed by an identifier
            if ((tk = maybe_want(self, TK_IDENTIFIER)))
                ;

            // Read enumerators if present
            if (maybe_want(self, TK_LCURLY))
                enumerator_list(self);

            continue;

        // FIXME: add typedef names

        // Type qualifier
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            break;

        // Function specifier
        case TK_INLINE:
            break;

        // Might be a typedef name
        case TK_IDENTIFIER:
            // Ignore identifier if we already have a type
            if (ty || had_ts)
                goto end_decl_specs;

            // Otherwise it might become a type
            if ((ty = sema_findtypedef(&self->sema, tk_str(tk))))
                break;

            // FALLTHROUGH

        // End of declaration specifiers
        default:
        end_decl_specs:

            if (!match)         // No type provided
                return NULL;

            if (ty)             // Struct, union, enum, or typedef
                return ty;

            return decode_ts(ts);
        }

        // Consume token
        adv(self);

        // Note that we've matched something
        match = true;
    }
}

void struct_declaration_list(cc3_t *self)
{
    do {
        int sc;
        ty_t *base_ty = declaration_specifiers(self, &sc);

        if (!base_ty)
            err("Expected declaration in struct");

        do {
            if (maybe_want(self, TK_COLON)) {   // Anonymous bitfield
                constant_expression(self);
                continue;
            }

            char *name;
            ty_t *ty = declarator(self, clone_ty(base_ty), false, &name);

            if (maybe_want(self, TK_COLON))
                constant_expression(self);

        } while (!maybe_want(self, TK_SEMICOLON));

        free_ty(base_ty);

    } while (!maybe_want(self, TK_RCURLY));
}

void enumerator_list(cc3_t *self)
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
                /*cnt = */constant_expression(self);
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

void initializer(cc3_t *self)
{
    if (maybe_want(self, TK_LCURLY))
        initializer_list(self);
    else
        assignment_expression(self);
}

void initializer_list(cc3_t *self)
{
    for (;;) {
        designation(self);
        initializer(self);

        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))    // Trailing comma allowed
                return;
        } else {
            want(self, TK_RCURLY);              // Otherwise the list must end
            return;
        }
    }
}

void designation(cc3_t *self)
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


/** Statements **/

static bool declaration(cc3_t *self)
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
            sema_declare(&self->sema, sc, ty, name);

            // Read initializer
            if (maybe_want(self, TK_AS))
                initializer(self);

        } while (maybe_want(self, TK_COMMA));

        // Declarators must end with ;
        want(self, TK_SEMICOLON);
    }

    free_ty(base_ty);

    return true;
}

static void statement(cc3_t *self);

static void block_item_list(cc3_t *self)
{
    sema_enter(&self->sema);
    while (!maybe_want(self, TK_RCURLY))
        if (!declaration(self))
            statement(self);
    sema_exit(&self->sema);
}

void statement(cc3_t *self)
{
    // Statements may be preceeded by labels

    if (peek(self, 0)->type == TK_IDENTIFIER && peek(self, 1)->type == TK_COLON) {
        adv(self);
        adv(self);
    } else if (maybe_want(self, TK_CASE)) {
        constant_expression(self);
        want(self, TK_COLON);
    } else if (maybe_want(self, TK_DEFAULT)) {
        want(self, TK_COLON);
    }

    // Compound statement
    if (maybe_want(self, TK_LCURLY)) {
        block_item_list(self);
        return;
    }

    // If statement
    if (maybe_want(self, TK_IF)) {
        // Heading
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);

        // Then
        statement(self);

        // Else
        if (maybe_want(self, TK_ELSE))
            statement(self);

        return;
    }

    // Switch statement
    if (maybe_want(self, TK_SWITCH)) {
        // Heading
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);

        // Body
        statement(self);

        return;
    }

    // While statement
    if (maybe_want(self, TK_WHILE)) {
        // Heading
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);

        // Body
        statement(self);

        return;
    }

    // Do ... while statement
    if (maybe_want(self, TK_DO)) {
        // Body
        statement(self);
        // Condition
        want(self, TK_WHILE);
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);
        want(self, TK_SEMICOLON);

        return;
    }

    // For statement
    if (maybe_want(self, TK_FOR)) {
        // Heading
        want(self, TK_LPAREN);

        // C99: for creates a scope of it's own
        sema_enter(&self->sema);

        if (!declaration(self))
            if (!maybe_want(self, TK_SEMICOLON)) {
                expression(self);
                want(self, TK_SEMICOLON);
            }

        if (!maybe_want(self, TK_SEMICOLON)) {
            expression(self);
            want(self, TK_SEMICOLON);
        }
        if (!maybe_want(self, TK_RPAREN)) {
            expression(self);
            want(self, TK_RPAREN);
        }

        // Body
        statement(self);

        // Exit for scope
        sema_exit(&self->sema);

        return;
    }

    // Jumps
    if (maybe_want(self, TK_GOTO)) {
        want(self, TK_IDENTIFIER);
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_CONTINUE)) {
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_BREAK)) {
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_RETURN)) {
        if (!maybe_want(self, TK_SEMICOLON)) {
            expression(self);
            want(self, TK_SEMICOLON);
        }
        return;
    }

    // Nothing else left, it must be an expression statement
    if (!maybe_want(self, TK_SEMICOLON)) {
        expression(self);
        want(self, TK_SEMICOLON);
    }
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
            sema_declare(&self->sema, sc, ty, name);

            // Check for function definition
            if (maybe_want(self, TK_LCURLY)) {
                block_item_list(self);
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
        }

        // Free base type
        free_ty(base_ty);

        // And the list must end with a ;
        want(self, TK_SEMICOLON);
    }
}
