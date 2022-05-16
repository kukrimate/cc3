// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

/** Compiler context **/

#define LOOKAHEAD_CNT 2

typedef struct cc3 cc3_t;

struct cc3 {
    // Lexical analyzer
    lexer_t lexer;

    // Token buffer (FIFO)
    tk_t tk_buf[LOOKAHEAD_CNT];
    int tk_pos, tk_cnt;

    // Semantic analyzer
    sema_t sema;
    // Code generator
    gen_t gen;
};

/** Initialization **/

static void cc3_init(cc3_t *self, int in_fd, int out_fd)
{
    lex_init(&self->lexer, in_fd);
    self->tk_pos = 0;
    self->tk_cnt = 0;
    for (int i = 0; i < LOOKAHEAD_CNT; ++i) {
        string_init(&self->tk_buf[i].spelling);
        string_init(&self->tk_buf[i].str);
    }
    sema_init(&self->sema);
    gen_init(&self->gen, out_fd);
}

static void cc3_free(cc3_t *self)
{
    lex_free(&self->lexer);
    for (int i = 0; i < LOOKAHEAD_CNT; ++i) {
        string_free(&self->tk_buf[i].spelling);
        string_init(&self->tk_buf[i].str);
    }
    sema_free(&self->sema);
    gen_free(&self->gen);
}

/** Interface to the lexer **/

static tk_t *peek(cc3_t *self, int i)
{
    // Make sure lookahead doesn't go too far
    assert(i < LOOKAHEAD_CNT);

    // Add tokens until we have enough
    while (self->tk_cnt <= i)
        lex_next(&self->lexer,
            self->tk_buf + (self->tk_pos + self->tk_cnt++) % LOOKAHEAD_CNT);

    // Return pointer to i-th token
    return self->tk_buf + (self->tk_pos + i) % LOOKAHEAD_CNT;
}

static void adv(cc3_t *self)
{
    // Make sure the buffer is not empty
    assert(self->tk_cnt > 0);

    // Skip over the front token
    self->tk_pos = (self->tk_pos + 1) % LOOKAHEAD_CNT;
    --self->tk_cnt;
}

static inline tk_t *next(cc3_t *self)
{
    tk_t *tk = peek(self, 0);
    adv(self);
    return tk;
}

static inline tk_t *want(cc3_t *self, int want_type)
{
    tk_t *tk = peek(self, 0);
    if (tk->type != want_type)
        err("Unexpected token %t", tk);
    adv(self);
    return tk;
}

static inline tk_t *maybe_want(cc3_t *self, int want_type)
{
    tk_t *tk = peek(self, 0);
    if (tk->type == want_type) {
        adv(self);
        return tk;
    }
    return NULL;
}

/** Syntax helpers **/

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

static bool is_type_name(cc3_t *self, tk_t *tk);
static stmt_t *read_block(cc3_t *self);
static ty_t *type_name(cc3_t *self);

expr_t *primary_expression(cc3_t *self)
{
    tk_t *tk = next(self);
    expr_t *expr;

    switch (tk->type) {
    case TK_IDENTIFIER:
        if (!strcmp(tk_str(tk), "__builtin_va_start")) {
            expr = alloc_expr(EXPR_VA_START);
            expr->ty = &ty_void;
            want(self, TK_LPAREN);
            expr->as_unary.arg = assignment_expression(self);
            want(self, TK_COMMA);
            // Per C standard we take the last non-variadic parameter as an
            // argument, however we already know which one it is, so we can
            // just discard it.
            assignment_expression(self);
            want(self, TK_RPAREN);
            break;
        }
        if (!strcmp(tk_str(tk), "__builtin_va_end")) {
            expr = alloc_expr(EXPR_VA_END);
            expr->ty = &ty_void;
            want(self, TK_LPAREN);
            expr->as_unary.arg = assignment_expression(self);
            want(self, TK_RPAREN);
            break;
        }
        if (!strcmp(tk_str(tk), "__builtin_va_arg")) {
            expr = alloc_expr(EXPR_VA_ARG);
            want(self, TK_LPAREN);
            expr->as_unary.arg = assignment_expression(self);
            want(self, TK_COMMA);
            expr->ty = type_name(self);
            want(self, TK_RPAREN);
            break;
        }

        expr = make_sym_expr(&self->sema, tk_str(tk));
        break;
    case TK_CONSTANT:
        expr = make_const_expr(&ty_int, tk->val);
        break;
    case TK_STR_LIT:
        expr = make_str_expr(strdup(tk->str.data));
        break;
    case TK_LPAREN:
        if (maybe_want(self, TK_LCURLY)) {  // [GNU]: statement expressions
            sema_enter(&self->sema);
            stmt_t *body = read_block(self);
            sema_exit(&self->sema);
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
            expr = make_dref_expr(make_add_expr(expr, expression(self)));
            want(self, TK_RSQ);
            continue;
        }
        if (maybe_want(self, TK_LPAREN)) {
            expr_vec_t args;
            expr_vec_init(&args);

            if (!maybe_want(self, TK_RPAREN)) {
                do
                    *expr_vec_push(&args) = assignment_expression(self);
                while (maybe_want(self, TK_COMMA));
                want(self, TK_RPAREN);
            }
            expr = make_call_expr(expr, &args);
            continue;
        }
        if (maybe_want(self, TK_DOT)) {
            const char *name = tk_str(want(self, TK_IDENTIFIER));
            expr = make_memb_expr(expr, name);
            continue;
        }
        if (maybe_want(self, TK_ARROW)) {
            const char *name = tk_str(want(self, TK_IDENTIFIER));
            expr = make_memb_expr(make_dref_expr(expr), name);
            continue;
        }
        if (maybe_want(self, TK_INCR)) {
            // FIXME: this isn't exactly the best way to do this
            expr = make_sub_expr(make_as_expr(expr, make_add_expr(expr,
                make_const_expr(&ty_int, 1))), make_const_expr(&ty_int, 1));
            continue;
        }
        if (maybe_want(self, TK_DECR)) {
            expr = make_add_expr(make_as_expr(expr, make_sub_expr(expr,
                make_const_expr(&ty_int, 1))), make_const_expr(&ty_int, 1));
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
        return make_as_expr(arg, make_add_expr(arg, make_const_expr(&ty_int, 1)));
    } else if (maybe_want(self, TK_DECR)) {
        expr_t *arg = unary_expression(self);
        return make_as_expr(arg, make_sub_expr(arg, make_const_expr(&ty_int, 1)));
    } else if (maybe_want(self, TK_AND)) {
        return make_ref_expr(cast_expression(self));
    } else if (maybe_want(self, TK_MUL)) {
        return make_dref_expr(cast_expression(self));
    } else if (maybe_want(self, TK_ADD)) {
        return make_pos_expr(cast_expression(self));
    } else if (maybe_want(self, TK_SUB)) {
        return make_neg_expr(cast_expression(self));
    } else if (maybe_want(self, TK_NOT)) {
        return make_not_expr(cast_expression(self));
    } else if (maybe_want(self, TK_LNOT)) {
        return make_lnot_expr(cast_expression(self));
    } else if (maybe_want(self, TK_SIZEOF)) {
        ty_t *ty;
        if (peek(self, 0)->type == TK_LPAREN && is_type_name(self, peek(self, 1))) {
            // sizeof '(' type-name ')'
            adv(self);
            ty = type_name(self);
            want(self, TK_RPAREN);
        } else {
            // sizeof unary-expression
            ty = unary_expression(self)->ty;
        }
        return make_sizeof_expr(ty);
    } else {
        return postfix_expression(self);
    }
}

expr_t *cast_expression(cc3_t *self)
{
    if (peek(self, 0)->type == TK_LPAREN && is_type_name(self, peek(self, 1))) {
        // '(' type-name ')' cast-expression
        adv(self);
        ty_t *ty = type_name(self);
        want(self, TK_RPAREN);
        return make_cast_expr(ty, cast_expression(self));
    } else {
        return unary_expression(self);
    }
}

expr_t *multiplicative_expression(cc3_t *self)
{
    expr_t *lhs = cast_expression(self);
    for (;;)
        if (maybe_want(self, TK_MUL))
            lhs = make_mul_expr(lhs, cast_expression(self));
        else if (maybe_want(self, TK_DIV))
            lhs = make_div_expr(lhs, cast_expression(self));
        else if (maybe_want(self, TK_MOD))
            lhs = make_mod_expr(lhs, cast_expression(self));
        else
            return lhs;
}

expr_t *additive_expression(cc3_t *self)
{
    expr_t *lhs = multiplicative_expression(self);
    for (;;)
        if (maybe_want(self, TK_ADD))
            lhs = make_add_expr(lhs, multiplicative_expression(self));
        else if (maybe_want(self, TK_SUB))
            lhs = make_sub_expr(lhs, multiplicative_expression(self));
        else
            return lhs;
}

expr_t *shift_expression(cc3_t *self)
{
    expr_t *lhs = additive_expression(self);
    for (;;)
        if (maybe_want(self, TK_LSH))
            lhs = make_lsh_expr(lhs, additive_expression(self));
        else if (maybe_want(self, TK_RSH))
            lhs = make_rsh_expr(lhs, additive_expression(self));
        else
            return lhs;
}

expr_t *relational_expression(cc3_t *self)
{
    expr_t *lhs = shift_expression(self);
    for (;;)
        if (maybe_want(self, TK_LT))
            lhs = make_lt_expr(lhs, shift_expression(self));
        else if (maybe_want(self, TK_GT))
            lhs = make_gt_expr(lhs, shift_expression(self));
        else if (maybe_want(self, TK_LE))
            lhs = make_le_expr(lhs, shift_expression(self));
        else if (maybe_want(self, TK_GE))
            lhs = make_ge_expr(lhs, shift_expression(self));
        else
            return lhs;
}

expr_t *equality_expression(cc3_t *self)
{
    expr_t *lhs = relational_expression(self);
    for (;;)
        if (maybe_want(self, TK_EQ))
            lhs = make_eq_expr(lhs, relational_expression(self));
        else if (maybe_want(self, TK_NE))
            lhs = make_ne_expr(lhs, relational_expression(self));
        else
            return lhs;
}

expr_t *and_expression(cc3_t *self)
{
    expr_t *lhs = equality_expression(self);
    while (maybe_want(self, TK_AND))
        lhs = make_and_expr(lhs, equality_expression(self));
    return lhs;
}

expr_t *xor_expression(cc3_t *self)
{
    expr_t *lhs = and_expression(self);
    while (maybe_want(self, TK_XOR))
        lhs = make_xor_expr(lhs, and_expression(self));
    return lhs;
}

expr_t *or_expression(cc3_t *self)
{
    expr_t *lhs = xor_expression(self);
    while (maybe_want(self, TK_OR))
        lhs = make_or_expr(lhs, xor_expression(self));
    return lhs;
}

expr_t *land_expression(cc3_t *self)
{
    expr_t *lhs = or_expression(self);
    while (maybe_want(self, TK_LAND))
        lhs = make_land_expr(lhs, or_expression(self));
    return lhs;
}

expr_t *lor_expression(cc3_t *self)
{
    expr_t *lhs = land_expression(self);
    while (maybe_want(self, TK_LOR))
        lhs = make_lor_expr(lhs, land_expression(self));
    return lhs;
}

expr_t *conditional_expression(cc3_t *self)
{
    expr_t *lhs = lor_expression(self);
    if (!maybe_want(self, TK_COND))
        return lhs;
    expr_t *mid = expression(self);
    want(self, TK_COLON);
    return make_cond_expr(lhs, mid, conditional_expression(self));
}

expr_t *assignment_expression(cc3_t *self)
{
    expr_t *lhs = conditional_expression(self);
    if (maybe_want(self, TK_AS))
        return make_as_expr(lhs, assignment_expression(self));
    else if (maybe_want(self, TK_MUL_AS))
        return make_as_expr(lhs,
                make_mul_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_DIV_AS))
        return make_as_expr(lhs,
                make_div_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_MOD_AS))
        return make_as_expr(lhs,
                make_mod_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_ADD_AS))
        return make_as_expr(lhs,
                make_add_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_SUB_AS))
        return make_as_expr(lhs,
                make_sub_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_LSH_AS))
        return make_as_expr(lhs,
                make_lsh_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_RSH_AS))
        return make_as_expr(lhs,
                make_rsh_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_AND_AS))
        return make_as_expr(lhs,
                make_and_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_XOR_AS))
        return make_as_expr(lhs,
                make_xor_expr(lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_OR_AS))
        return make_as_expr(lhs,
                make_or_expr(lhs, assignment_expression(self)));
    else
        return lhs;
}

expr_t *expression(cc3_t *self)
{
    expr_t *lhs = assignment_expression(self);
    while (maybe_want(self, TK_COMMA))
        lhs = make_seq_expr(lhs, assignment_expression(self));
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

static void scalar_initialzier(cc3_t *self, init_t *out, ty_t *ty)
{
    // If we reach a member of an aggregate without an initializer, we need to
    // zero initialize it. We make that explicit to simplify code generation.
    if (peek(self, 0)->type == TK_RCURLY)
        return make_init_expr(out, ty, make_const_expr(&ty_int, 0));

    if (maybe_want(self, TK_LCURLY)) {
        // Recurse if the initializer is enclosed in braces
        scalar_initialzier(self, out, ty);
        want(self, TK_RCURLY);
    } else {
        // Otherwise there must be an expression here
        make_init_expr(out, ty, assignment_expression(self));
    }
}

static bool string_array_initializer(cc3_t *self, init_t *out, ty_t *ty)
{
    // Bail on non-character arrays
    assert(ty->kind == TY_ARRAY);
    if (!(ty->array.elem_ty->kind & (TY_CHAR | TY_SCHAR | TY_UCHAR)))
        return false;

    // Bail if the next token is not a string literal
    // FIXME: the string literal might be enclosed in braces
    tk_t *tk;
    if (!(tk = maybe_want(self, TK_STR_LIT)))
        return false;

    // The string literal might complete the array's type
    if (ty->array.cnt == -1) {
        int cnt = tk->str.length + 1;
        ty->align = ty->array.elem_ty->align;
        ty->size = ty->array.elem_ty->size * cnt;
        ty->array.cnt = cnt;
    }

    // Create an initializer list from the string literal
    init_vec_t list;
    init_vec_init(&list);

    for (int i = 0; i < ty->array.cnt; ++i)
        if (i < tk->str.length)
            make_init_expr(init_vec_push(&list), ty->array.elem_ty,
                make_const_expr(&ty_char, tk->str.data[i]));
        else
            make_init_expr(init_vec_push(&list), ty->array.elem_ty,
                make_const_expr(&ty_char, 0));

    make_init_list(out, &list);
    return true;
}

static void initializer_r(cc3_t *self, init_t *out, ty_t *ty, bool outer);

static void struct_initializer(cc3_t *self, init_t *out, ty_t *ty)
{
    init_vec_t list;
    init_vec_init(&list);

    // Bind an initializer to each member of the struct
    for (memb_t *memb = ty->as_aggregate.members; memb; memb = memb->next)
        initializer_r(self, init_vec_push(&list), memb->ty, false);

    // Return the new initializer list we've built
    make_init_list(out, &list);
}

static void union_initializer(cc3_t *self, init_t *out, ty_t *ty)
{
    // Like the struct initializer above, however only the first member
    // can be initialized of an union

    init_vec_t list;
    init_vec_init(&list);
    initializer_r(self, init_vec_push(&list), ty->as_aggregate.members->ty, false);

    make_init_list(out, &list);
}

static void array_initializer(cc3_t *self, init_t *out, ty_t *ty)
{
    init_vec_t list;
    init_vec_init(&list);

    if (ty->array.cnt == -1) {
        // Bind as many initializers as we can, and count them
        int cnt;
        for (cnt = 0; peek(self, 0)->type != TK_RCURLY; ++cnt)
            initializer_r(self, init_vec_push(&list), ty->array.elem_ty, false);

        // Then complete the array type with the number of entries
        ty->align = ty->array.elem_ty->align;
        ty->size = ty->array.elem_ty->size * cnt;
        ty->array.cnt = cnt;
    } else {
        // Bind an initializer to each array element
        for (int i = 0; i < ty->array.cnt; ++i)
            initializer_r(self, init_vec_push(&list), ty->array.elem_ty, false);
    }

    // Return the new initializer list we've built
    make_init_list(out, &list);
}

static void initializer_r(cc3_t *self, init_t *out, ty_t *ty, bool outer)
{
    switch (ty->kind) {
    // Struct:
    // - a brace enclosed initializer list
    // - consume elements from the outer initializer list
    case TY_STRUCT:
        if (maybe_want(self, TK_LCURLY)) {
            struct_initializer(self, out, ty);
            want(self, TK_RCURLY);
            break;
        }

        // Otherwise consume elements from the outer initializer list
        if (outer)
            err("Invalid initializer for struct %T", ty);

        struct_initializer(self, out, ty);
        break;

    // Union:
    // - a brace enclosed initializer list
    // - consume elements from the outer initializer list
    case TY_UNION:
        if (maybe_want(self, TK_LCURLY)) {
            union_initializer(self, out, ty);
            want(self, TK_RCURLY);
            break;
        }

        // Otherwise consume elements from the outer initializer list
        if (outer)
            err("Invalid initializer for union %T", ty);

        union_initializer(self, out, ty);
        break;

    // Array:
    // - for char arrays, a string literal, optionally in braces
    // - a brace enclosed initializer list
    // - consume elements from the outer initializer list
    case TY_ARRAY:
        if (ty->array.cnt == -1 && !outer)
            err("Only the outermost array type is allowed to be incomplete");

        // First see if we found a string literal initializer
        if (string_array_initializer(self, out, ty))
            break;

        if (maybe_want(self, TK_LCURLY)) {
            array_initializer(self, out, ty);
            want(self, TK_RCURLY);
            break;
        }

        // Otherwise consume elements from the outer initializer list
        if (outer)
            err("Invalid initializer for array %T", ty);

        array_initializer(self, out, ty);
        break;

    // Scalar:
    // - an expression, optionally in braces
    default:
        scalar_initialzier(self, out, ty);
    }

    // Initializer list members might be followed by a comma
    if (!outer)
        maybe_want(self, TK_COMMA);
}

static void initializer(cc3_t *self, init_t *out, ty_t *ty)
{
    initializer_r(self, out, ty, true);
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
                }*/

                char *name;
                ty_t *ty = declarator(self, base_ty, false, &name);

                /* FIXME: bitfield
                if (maybe_want(self, TK_COLON))
                    constant_expression(self);*/

                *tail = make_memb(ty, name);
                tail = &(*tail)->next;
            } while (maybe_want(self, TK_COMMA));   // Another declarator after ,
            want(self, TK_SEMICOLON);               // Must end with ;
        } else {
            // [GNU] An anonymous struct/union is allowed as a member
            if (base_ty->kind != TY_STRUCT && base_ty->kind != TY_UNION)
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

    do {
        // Read enumerator name
        char *name = strdup(tk_str(want(self, TK_IDENTIFIER)));
        // Optionally there might be a value
        if (maybe_want(self, TK_AS))
            cur = constant_expression(self);
        // Declare enumeration constant
        sema_declare_enum_const(&self->sema, name, cur++);
    } while (!end_comma_separated(self));
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

/*
 * Turn declaration specifiers into into an actual type
 */

ty_t *declaration_specifiers(cc3_t *self, int *out_sc)
{
    enum {
        TS_VOID, TS_CHAR, TS_SHORT, TS_INT, TS_LONG, TS_FLOAT, TS_DOUBLE,
        TS_SIGNED, TS_UNSIGNED, TS_BOOL, TS_COMPLEX, TS_IMAGINARY, NUM_TS
    };

    static const struct {
        int ts[NUM_TS];
        ty_t *ty;
    } ts_map[] = {
        //  V  C  S  I  L  F  D  S  U  B  C  I
        { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, &ty_void      },  // void
        { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, }, &ty_bool      },  // _Bool
        { { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, &ty_char      },  // char
        { { 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, }, &ty_schar     },  // signed char
        { { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, }, &ty_uchar     },  // unsigned char
        { { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, &ty_short     },  // short
        { { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, }, &ty_short     },  // signed short
        { { 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, }, &ty_short     },  // short int
        { { 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, }, &ty_short     },  // signed short int
        { { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, }, &ty_ushort    },  // unsigned short
        { { 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, }, &ty_ushort    },  // unsigned short int
        { { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, }, &ty_int       },  // int
        { { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, }, &ty_int       },  // signed
        { { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, }, &ty_int       },  // signed int
        { { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, }, &ty_uint      },  // unsigned
        { { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, }, &ty_uint      },  // unsigned int
        { { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, }, &ty_long      },  // long
        { { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, }, &ty_long      },  // signed long
        { { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, }, &ty_long      },  // long int
        { { 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, }, &ty_long      },  // signed long int
        { { 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, }, &ty_ulong     },  // unsigned long
        { { 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, }, &ty_ulong     },  // unsigned long int
        { { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, }, &ty_llong     },  // long long
        { { 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, }, &ty_llong     },  // signed long long
        { { 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, }, &ty_llong     },  // long long int
        { { 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 0, }, &ty_llong     },  // signed long long int
        { { 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, }, &ty_ullong    },  // unsigned long long
        { { 0, 0, 0, 1, 2, 0, 0, 0, 1, 0, 0, 0, }, &ty_ullong    },  // unsigned long long int
        { { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, }, &ty_float     },  // float
        { { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, }, &ty_double    },  // double
        { { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, }, &ty_ldouble   },  // long double
    };

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
            if (*out_sc != -1) goto err_sc;
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
        #define INCR_TS(idx)                        \
            do {                                    \
                if (ty) goto err_ts;                \
                had_ts = true;                      \
                ++ts[idx];                          \
            } while (0)

        case TK_VOID:       INCR_TS(TS_VOID);       break;
        case TK_CHAR:       INCR_TS(TS_CHAR);       break;
        case TK_SHORT:      INCR_TS(TS_SHORT);      break;
        case TK_INT:        INCR_TS(TS_INT);        break;
        case TK_LONG:       INCR_TS(TS_LONG);       break;
        case TK_FLOAT:      INCR_TS(TS_FLOAT);      break;
        case TK_DOUBLE:     INCR_TS(TS_DOUBLE);     break;
        case TK_SIGNED:     INCR_TS(TS_SIGNED);     break;
        case TK_UNSIGNED:   INCR_TS(TS_UNSIGNED);   break;
        case TK_BOOL:       INCR_TS(TS_BOOL);       break;
        case TK_COMPLEX:    INCR_TS(TS_COMPLEX);    break;
        case TK_IMAGINARY:  INCR_TS(TS_IMAGINARY);  break;

        #undef INCR_TS

        case TK_STRUCT:
            if (ty || had_ts) goto err_ts;
            adv(self);
            ty = struct_specifier(self);
            continue;
        case TK_UNION:
            if (ty || had_ts) goto err_ts;
            adv(self);
            ty = union_specifier(self);
            continue;
        case TK_ENUM:
            if (ty || had_ts) goto err_ts;
            adv(self);
            enum_specifier(self);
            ty = &ty_int;
            continue;

        case TK_TYPEOF:
            if (ty || had_ts) goto err_ts;
            adv(self);
            want(self, TK_LPAREN);
            if (is_type_name(self, peek(self, 0)))
                ty = type_name(self);
            else
                ty = expression(self)->ty;
            want(self, TK_RPAREN);
            continue;

        case TK_IDENTIFIER:
            // Typedef name have to be the only type specifier present
            if (!ty && !had_ts)
                if ((ty = sema_findtypedef(&self->sema, tk_str(tk))))
                    break;

            FALLTHROUGH;

        default:
            // Struct, union, enum, or typedef
            if (ty)
                return ty;

            // Normal multiset of type specifiers
            if (had_ts) {
                for (int i = 0; i < ARRAY_SIZE(ts_map); ++i)
                    if (!memcmp(ts, ts_map[i].ts, sizeof ts_map[i].ts))
                        return ts_map[i].ty;
                goto err_ts;
            }

            // No type specifiers
            return NULL;
        }

        // Consume token
        adv(self);
    }

    err_sc: err("Duplicate storage class");
    err_ts: err("Invalid type specifiers");
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

typedef struct decl decl_t;

enum {
    DECL_NAME,
    DECL_POINTER,
    DECL_ARRAY,
    DECL_FUNCTION,
};

struct decl {
    decl_t *next;

    int kind;

    union {
        char *as_name;

        struct {
            int cnt;
        } as_array;

        struct {
            scope_t *scope;
            param_t *params;
            bool var;
        } as_function;
    };
};

static decl_t *make_decl(decl_t *next, int kind)
{
    decl_t *decl = calloc(1, sizeof *decl);
    if (!decl) abort();
    decl->next = next;
    decl->kind = kind;
    return decl;
}

static decl_t *declarator_r(cc3_t *self)
{
    if (maybe_want(self, TK_MUL)) {                         // Pointer
        type_qualifier_list(self);
        return make_decl(declarator_r(self), DECL_POINTER);
    }

    tk_t *tk;
    decl_t *decl;

    // FIXME: '(' typedef-name ')' is a function suffix not an IDENTIFIER

    if (maybe_want(self, TK_LPAREN)) {                      // '(' declarator ')'
        decl = declarator_r(self);
        want(self, TK_RPAREN);
    } else if ((tk = maybe_want(self, TK_IDENTIFIER))) {    // IDENTIFIER
        decl = make_decl(NULL, DECL_NAME);
        decl->as_name = strdup(tk_str(tk));
    } else {                                                // Abstract
        decl = NULL;
    }

    for (;;)
        if (maybe_want(self, TK_LSQ)) {                     // Array
            type_qualifier_list(self);
            maybe_want(self, TK_STATIC);
            type_qualifier_list(self);
            decl = make_decl(decl, DECL_ARRAY);
            if (maybe_want(self, TK_RSQ)) {
                decl->as_array.cnt = -1;
            } else {
                decl->as_array.cnt = constant_expression(self);
                want(self, TK_RSQ);
            }
        } else if (maybe_want(self, TK_LPAREN)) {            // Function
            decl = make_decl(decl, DECL_FUNCTION);
            sema_enter(&self->sema);
            if (peek(self, 0)->type == TK_VOID && peek(self, 1)->type == TK_RPAREN) {
                // "void" as the only unnamed parameter means no parameters
                adv(self);
                adv(self);
            } else {
                // Otherwise a parameter-type-list follows
                for (param_t **tail = &decl->as_function.params;;) {
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
                        ty = make_pointer(ty->array.elem_ty);
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
                        decl->as_function.var = true;
                        break;
                    }
                }
                want(self, TK_RPAREN);
            }
            decl->as_function.scope = sema_pop(&self->sema);
        } else {
            return decl;
        }
}

static ty_t *declarator(cc3_t *self, ty_t *ty, bool allow_abstract, char **out_name)
{
    // Iterate declarators until we reach a name
    for (decl_t *decl = declarator_r(self); decl; decl = decl->next)
        switch (decl->kind) {
        case DECL_NAME:
            *out_name = decl->as_name;
            return ty;
        case DECL_POINTER:
            ty = make_pointer(ty);
            break;
        case DECL_ARRAY:
            ty = make_array(ty, decl->as_array.cnt);
            break;
        case DECL_FUNCTION:
            ty = make_function(ty,
                    decl->as_function.scope,
                    decl->as_function.params,
                    decl->as_function.var);
            break;
        default:
            ASSERT_NOT_REACHED();
        }

    // If there was no name it must be an abstract declarator
    if (!allow_abstract)
        err("Expected named declarator");

    // We mark abstract declarators by setting the name to NULL
    *out_name = NULL;

    return ty;
}

static bool is_type_name(cc3_t *self, tk_t *tk)
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
    case TK_TYPEOF:
        return true;

    // Might be a typedef name
    case TK_IDENTIFIER:
        return sema_findtypedef(&self->sema, tk_str(tk)) != NULL;

    default:
        return false;
    }
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

            if (sym->kind == SYM_LOCAL) {
                if (maybe_want(self, TK_AS)) {
                    // Locals are intermixed with code and represented in the AST
                    stmt_t *stmt = make_stmt(STMT_DECL);
                    stmt->as_decl.sym = sym;
                    initializer(self, &stmt->as_decl.init, sym->ty);
                    append_stmt(tail, stmt);
                }

                sema_alloc_local(&self->sema, sym);
            } else {
                if (maybe_want(self, TK_AS)) {
                    // Static initializers get generated in the .data section
                    init_t init;
                    initializer(self, &init, sym->ty);
                    gen_static(&self->gen, sym, &init);
                }
            }
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

static void function_definition(cc3_t *self, int sc, ty_t *ty, char *name)
{
    sym_t *sym = sema_declare(&self->sema, sc, ty, name);

    // Check for function definition
    // Make sure it is really a function
    if (ty->kind != TY_FUNCTION)
        err("Expected function type instead of %T", ty);

    // HACK!: zero sema's picture of the frame size before
    // entering the context of a new function
    self->sema.func_name = sym->name;
    self->sema.offset = 0;

    // NOTE: if we are dealing with a varargs function, we *must*
    // leave 48 bytes as a register save area
    if (ty->function.var)
        self->sema.offset = 48;

    // Now we can allocate space in the stack frame for the
    // parameters of the current function
    int gp = 0;

    for (param_t *param = ty->function.params; param; param = param->next) {
        if (!param->sym)
            err("Unnamed parameters are not allowed in function definitions");
        if (gp < 6) {
            sema_alloc_local(&self->sema, param->sym);
            ++gp;
        }
    }

    // Bring parameters into scope for parsing the function body
    sema_push(&self->sema, ty->function.scope);
    stmt_t *body = read_block(self);
    sema_exit(&self->sema);

    // Dump function body for debugging
    printf("Defined functions %s\n", name);
    print_stmts(body, 1);

    // Then we can generate the function, providing the correct
    // frame size from the hack above
    gen_func(&self->gen, sym, self->sema.offset, body);
}

static void external_declaration(cc3_t *self, int sc, ty_t *ty, char *name)
{
    sym_t *sym = sema_declare(&self->sema, sc, ty, name);

    if (maybe_want(self, TK_AS)) {
        init_t init;
        initializer(self, &init, sym->ty);
        gen_static(&self->gen, sym, &init);
    }
}

static void translation_unit(cc3_t *self)
{
    // Read function definitions and declarations
    for (;;) {
        int sc;
        ty_t *base_ty = declaration_specifiers(self, &sc);
        if (!base_ty)                                       // End of file
            break;
        if (maybe_want(self, TK_SEMICOLON))                 // No names declared
            continue;

        char *name;
        ty_t *ty = declarator(self, base_ty, false, &name);

        if (maybe_want(self, TK_LCURLY)) {                  // function-definition
            function_definition(self, sc, ty, name);
        } else {                                            // external-declaration
            external_declaration(self, sc, ty, name);
            while (maybe_want(self, TK_COMMA)) {
                char *name;
                ty_t *ty = declarator(self, base_ty, false, &name);
                external_declaration(self, sc, ty, name);
            }
            want(self, TK_SEMICOLON);
        }
    }
    want(self, TK_EOF);

    // Dump out the string literals we have collected
    gen_lits(&self->gen);
}

/** External API **/

void cc3_compile(int in_fd, int out_fd)
{
    cc3_t ctx;
    cc3_init(&ctx, in_fd, out_fd);
    translation_unit(&ctx);
    cc3_free(&ctx);
}
