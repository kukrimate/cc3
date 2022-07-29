// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

// Primitive type table

ty_t ty_void    = { TY_VOID                     };
ty_t ty_bool    = { TY_BOOL,    1,      1,      };
ty_t ty_char    = { TY_CHAR,    1,      1,      };
ty_t ty_schar   = { TY_SCHAR,   1,      1,      };
ty_t ty_uchar   = { TY_UCHAR,   1,      1,      };
ty_t ty_short   = { TY_SHORT,   2,      2,      };
ty_t ty_ushort  = { TY_USHORT,  2,      2,      };
ty_t ty_int     = { TY_INT,     4,      4,      };
ty_t ty_uint    = { TY_UINT,    4,      4,      };
ty_t ty_long    = { TY_LONG,    8,      8,      };
ty_t ty_ulong   = { TY_ULONG,   8,      8,      };
ty_t ty_llong   = { TY_LLONG,   8,      8,      };
ty_t ty_ullong  = { TY_ULLONG,  8,      8,      };
ty_t ty_float   = { TY_FLOAT,   4,      4,      };
ty_t ty_double  = { TY_DOUBLE,  8,      8,      };
ty_t ty_ldouble = { TY_LDOUBLE, 16,     16      };

// va_list type, as defined by the SysV AMD64 ABI

// typedef struct {
//     unsigned int gp_offset;
//     unsigned int fp_offset;
//     void *overflow_arg_area;
//     void *reg_save_area;
// } va_list[1];

ty_t ty_va_list = { TY_ARRAY, 8, 24, {{ &ty_void, 1 }} };

ty_t *make_ty(int kind)
{
    ty_t *ty = calloc(1, sizeof *ty);
    if (!ty) abort();
    ty->kind = kind;
    return ty;
}

ty_t *make_pointer(ty_t *base_ty)
{
    ty_t *ty = make_ty(TY_POINTER);
    ty->align = 8;
    ty->size = 8;
    ty->pointer.base_ty = base_ty;
    return ty;
}

ty_t *make_array(ty_t *elem_ty, int cnt)
{
    if (elem_ty->kind == TY_VOID)
        err("Array of void");
    if (elem_ty->kind == TY_ARRAY && elem_ty->array.cnt == -1)
        err("Array of incomplete array %T", elem_ty);
    if (elem_ty->kind & (TY_STRUCT | TY_UNION))
        if (!elem_ty->as_aggregate.had_def)
            err("Array of incomplete aggregate %T", elem_ty);
    if (elem_ty->kind == TY_FUNCTION)
        err("Array cannot contain function %T", elem_ty);

    ty_t *ty = make_ty(TY_ARRAY);
    if (cnt != -1) {
        ty->align = elem_ty->align;
        ty->size = elem_ty->size * cnt;
    }
    ty->array.elem_ty = elem_ty;
    ty->array.cnt = cnt;
    return ty;
}

ty_t *make_function(ty_t *ret_ty, scope_t *scope, param_vec_t *params, bool var)
{
    if (ret_ty->kind == TY_ARRAY)
        err("Function returning array %T", ret_ty);
    if (ret_ty->kind & (TY_STRUCT | TY_UNION))
        if (!ret_ty->as_aggregate.had_def)
            err("Function returning incomplete aggregate %T", ret_ty);
    if (ret_ty->kind == TY_FUNCTION)
        err("Function returning function %T", ret_ty);

    ty_t *ty = make_ty(TY_FUNCTION);
    ty->function.ret_ty = ret_ty;
    ty->function.scope = scope;
    ty->function.params = *params;
    ty->function.var = var;
    return ty;
}

static bool compare_ty(ty_t *ty1, ty_t *ty2)
{
    switch (ty1->kind) {
    case TY_VOID:
    case TY_BOOL:
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_SHORT:
    case TY_USHORT:
    case TY_INT:
    case TY_UINT:
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
        return ty1->kind == ty2->kind;

    case TY_UNION:
    case TY_STRUCT:
        return ty1 == ty2;

    case TY_POINTER:
        if (ty2->kind != TY_POINTER)
            return false;
        return compare_ty(ty1->pointer.base_ty, ty2->pointer.base_ty);

    case TY_ARRAY:
        if (ty2->kind != TY_ARRAY)
            return false;
        if (ty1->array.cnt != ty2->array.cnt)
            return false;
        return compare_ty(ty1->array.elem_ty, ty2->array.elem_ty);

    case TY_FUNCTION:
        if (ty2->kind != TY_FUNCTION)
            return false;
        if (ty1->function.var != ty2->function.var)
            return false;
        if (ty1->function.params.length != ty2->function.params.length)
            return false;
        if (!compare_ty(ty1->function.ret_ty, ty2->function.ret_ty))
            return false;
        for (int i = 0; i < ty1->function.params.length; ++i)
            if (!compare_ty(VEC_AT(&ty1->function.params, i)->ty,
                            VEC_AT(&ty2->function.params, i)->ty))
            return false;
        return true;

    default:
        ASSERT_NOT_REACHED();
    }
}

void sema_init(sema_t *self)
{
    self->scope = NULL;
    sema_enter(self);
    self->block_static_cnt = 0;
}

void sema_free(sema_t *self)
{
    sema_exit(self);
}

void sema_enter(sema_t *self)
{
    scope_t *scope = calloc(1, sizeof *scope);
    if (!scope) abort();
    scope->parent = self->scope;
    self->scope = scope;
    map_init(&scope->syms);
    map_init(&scope->tags);
}

void sema_exit(sema_t *self)
{
    scope_t *scope = self->scope;
    self->scope = scope->parent;
    map_free(&scope->syms);
    map_free(&scope->tags);
}

void sema_push(sema_t *self, scope_t *scope)
{
    scope->parent = self->scope;
    self->scope = scope;
}

scope_t *sema_pop(sema_t *self)
{
    scope_t *scope = self->scope;
    self->scope = scope->parent;
    return scope;
}

static int sym_kind(sema_t *self, int sc)
{
    switch (sc) {
    case TK_TYPEDEF:
        return SYM_TYPEDEF;
    case TK_EXTERN:
        return SYM_EXTERN;
    case TK_STATIC:
        return SYM_STATIC;
    case TK_AUTO:
    case TK_REGISTER:
        return SYM_LOCAL;
    case -1:
        if (self->scope->parent)
            return SYM_LOCAL;
        else
            return SYM_EXTERN;
    default:
        ASSERT_NOT_REACHED();
    }
}

sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, const char *name)
{
    // debugln("Declare %s: %T", name, ty);

    // Figure out the symbol kind
    int kind = sym_kind(self, sc);

    // Look for previous declaration in the current scope
    bool found;
    entry_t *entry = map_find_or_insert(&self->scope->syms, name, &found);

    if (found) {
        sym_t *sym = entry->as_sym;

        switch (sym->kind) {
        case SYM_EXTERN:
        case SYM_STATIC:
            if (kind == SYM_EXTERN) { // OK
                break;
            }
            if (kind == SYM_STATIC) { // OK
                sym->kind = SYM_STATIC;
                break;
            }
            FALLTHROUGH;
        default:
            err("Invalid re-declaration of %s", name);
        }

        // FIXME: check re-declaration type
        // NOTE: This assignment is needed for function defs
        sym->ty = ty;

        return sym;
    }

    // Create symbol
    sym_t *sym = calloc(1, sizeof *sym);
    if (!sym) abort();
    sym->kind = sym_kind(self, sc);
    sym->ty = ty;
    sym->name = name;

    // Create unique assembler name for block scope static
    if (sym->kind == SYM_STATIC && self->scope->parent) {
        string_t s;
        string_init(&s);
        string_printf(&s, "%s$%d", sym->name, self->block_static_cnt++);
        sym->asm_name = s.data;
    } else {
        sym->asm_name = sym->name;
    }

    // Add it to the symbol table
    entry->as_sym = sym;

    return sym;
}

sym_t *sema_declare_enum_const(sema_t *self, const char *name, val_t val)
{
    // Look for previous declaration in the current scope
    bool found;
    entry_t *entry = map_find_or_insert(&self->scope->syms, name, &found);

    if (found)
        err("Re-declaration of enumeration constant %s", name);

    // Create symbol
    sym_t *sym = calloc(1, sizeof *sym);
    if (!sym) abort();
    sym->kind = SYM_ENUM_CONST;
    sym->name = name;
    sym->val = val;

    // Declare it in the current scope
    entry->as_sym = sym;

    return sym;
}

sym_t *sema_lookup(sema_t *self, const char *name)
{
    for (scope_t *scope = self->scope; scope; scope = scope->parent) {
        entry_t *entry = map_find(&scope->syms, name);
        if (entry)
            return entry->as_sym;
    }
    return NULL;
}

ty_t *sema_findtypedef(sema_t *self, const char *name)
{
    sym_t *sym = sema_lookup(self, name);
    if (sym && sym->kind == SYM_TYPEDEF)
        return sym->ty;
    return NULL;
}

ty_t *sema_forward_declare_tag(sema_t *self, int kind, const char *name)
{
    // Look for name in any scope
    entry_t *entry = NULL;
    for (scope_t *scope = self->scope; scope; scope = scope->parent)
        if ((entry = map_find(&scope->tags, name)))
            break;

    if (entry) {
        // Make sure the right keyword is used to refer to the tag
        if (entry->as_ty->kind != kind)
            err("Tag referred to with the wrong keyword");
        // Return the type associated with the tag we found
        return entry->as_ty;
    }

    bool found;
    entry = map_find_or_insert(&self->scope->tags, name, &found);
    assert(!found);

    // Create type
    ty_t *ty = make_ty(kind);
    ty->as_aggregate.name = name;

    // Add it to the current scope
    entry->key = ty->as_aggregate.name;
    entry->as_ty = ty;

    return ty;
}

ty_t *sema_define_tag(sema_t *self, int kind, const char *name)
{
    // Look for name just in the current scope
    bool found;
    entry_t *entry = map_find_or_insert(&self->scope->tags, name, &found);

    if (found) {
        // Make sure the right keyword is used to refer to the tag
        if (entry->as_ty->kind != kind)
            err("Tag referred to with the wrong keyword");
        // Return the type associated with the tag we found
        return entry->as_ty;
    }

    // Create new type
    ty_t *ty = make_ty(kind);
    ty->as_aggregate.name = name;
    // Add it to the current scope
    entry->key = ty->as_aggregate.name;
    entry->as_ty = ty;
    return ty;
}

expr_t *alloc_expr(int kind)
{
    expr_t *expr = calloc(1, sizeof *expr);
    if (!expr) abort();
    expr->kind = kind;
    return expr;
}

//
// Primary expressions
//

expr_t *make_sym_expr(sema_t *self, const char *name)
{
    // Special pre-declared identifier
    if (self->func_name && !strcmp(name, "__func__"))
        return make_str_expr(self->func_name);

    // Find symbol
    sym_t *sym = sema_lookup(self, name);
    if (!sym)
        err("Undeclared identifier %s", name);

    // Create expression based on symbol type
    expr_t *expr;

    switch (sym->kind) {
    case SYM_TYPEDEF:
        err("Invalid use of typedef name %s", name);
    case SYM_EXTERN:
    case SYM_STATIC:
    case SYM_LOCAL:
        expr = alloc_expr(EXPR_SYM);
        expr->ty = sym->ty;
        expr->as_sym = sym;
        break;
    case SYM_ENUM_CONST:
        expr = make_const_expr(&ty_int, sym->val);
        break;
    }

    return expr;
}

expr_t *make_const_expr(ty_t *ty, val_t value)
{
    expr_t *expr = alloc_expr(EXPR_CONST);
    expr->ty = ty;
    expr->as_const.value = value;
    return expr;
}

expr_t *make_str_expr(const char *data)
{
    expr_t *expr = alloc_expr(EXPR_STR);
    expr->ty = make_array(&ty_char, strlen(data) + 1);
    expr->as_str.data = data;
    return expr;
}

// Is an expression a NULL pointer constant?

static bool is_null_const(expr_t *expr)
{
    return expr->ty->kind & TY_INT_MASK
            && expr->kind == EXPR_CONST
            && expr->as_const.value == 0;
}

// Degrade array and function types to pointers per (6.3.2.1)

static ty_t *find_ptr_base(ty_t *ty)
{
    if (ty->kind == TY_POINTER)
        return ty->pointer.base_ty;
    if (ty->kind == TY_ARRAY)
        return ty->array.elem_ty;
    if (ty->kind == TY_FUNCTION)
        return ty;
    return NULL;
}

// Compare two pointers, taking degradation into account

static ty_t *compare_ptrs(ty_t *ty1, ty_t *ty2, bool permit_void)
{
    if (!(ty1 = find_ptr_base(ty1)))
        return NULL;
    if (!(ty2 = find_ptr_base(ty2)))
        return NULL;
    if (permit_void) {
        if (ty1->kind == TY_VOID)
            return ty2;
        if (ty2->kind == TY_VOID)
            return ty1;
    }
    if (!compare_ty(ty1, ty2))
        return NULL;
    return ty1;
}

//
// Postfix expressions
//

static int find_memb(ty_t *ty, const char *name, ty_t **out)
{
    if (ty->kind != TY_STRUCT && ty->kind != TY_UNION)
        return -1;

    VEC_FOREACH(&ty->as_aggregate.members, memb) {
        if (!memb->name) {
            int result = find_memb(memb->ty, name, out);
            if (result != -1)
                return memb->offset + result;
        } else if (!strcmp(memb->name, name)) {
            *out = memb->ty;
            return memb->offset;
        }
    }

    return -1;
}

expr_t *make_memb_expr(expr_t *aggr, const char *name)
{
    expr_t *expr = alloc_expr(EXPR_MEMB);
    expr->as_memb.aggr = aggr;
    expr->as_memb.name = name;
    expr->as_memb.offset = find_memb(aggr->ty, name, &expr->ty);
    if (expr->as_memb.offset == -1)
        err("Non-existent member %s of type %T", name, aggr->ty);
    return expr;
}

static expr_t *convert_by_assignment(ty_t *dest_ty, expr_t *src)
{
    if (dest_ty->kind & TY_ARITH_MASK && src->ty->kind & TY_ARITH_MASK) {
        if (dest_ty->kind != src->ty->kind)
            return make_cast_expr(dest_ty, src);
        else
            return src;
    }

    if (dest_ty->kind & (TY_STRUCT | TY_UNION) && dest_ty == src->ty)
        return src;

    if (dest_ty->kind == TY_POINTER && compare_ptrs(dest_ty, src->ty, true))
        return src;

    if (dest_ty->kind == TY_POINTER && is_null_const(src))
        return make_cast_expr(dest_ty, src);

    if (dest_ty->kind == TY_BOOL && find_ptr_base(src->ty))
        return make_cast_expr(dest_ty, src);

    err("Cannot convert type %T to %T by assignment", src->ty, dest_ty);
}

expr_t *make_call_expr(expr_t *func, expr_vec_t *args)
{

    // Called expression must be a function pointer after implicit conversions
    ty_t *func_ty = find_ptr_base(func->ty);
    if (!func_ty)
        err("Cannot call %T object", func->ty);


    // Convert the arguments to the type of the corresponding parameters
    param_t *param = VEC_BEGIN(&func_ty->function.params),
            *end_param = VEC_END(&func_ty->function.params);

    VEC_FOREACH(args, arg)
        if (param < end_param) {
            // Function arguments get converted "as if by assignment"
            *arg = convert_by_assignment(param++->ty, *arg);
        } else if (func_ty->function.var) {
            // Variable arguments get the "default argument promotions"
            if ((*arg)->ty->kind & TY_PROMOTE_MASK)
                *arg = make_cast_expr(&ty_int, *arg);
            else if ((*arg)->ty->kind == TY_FLOAT)
                *arg = make_cast_expr(&ty_double, *arg);
        } else {
            err("Too many arguments for %T", func_ty);
        }
    if (param < end_param)
        err("Not enough arguments for %T", func_ty);

    expr_t *expr = alloc_expr(EXPR_CALL);
    expr->ty = func_ty->function.ret_ty;
    expr->as_call.func = func;
    expr->as_call.args = *args;
    return expr;
}

//
// Prefix expressions
//

expr_t *make_ref_expr(expr_t *arg)
{
    expr_t *expr = alloc_expr(EXPR_REF);
    expr->ty = make_pointer(arg->ty);
    expr->as_unary.arg = arg;
    return expr;
}

expr_t *make_dref_expr(expr_t *arg)
{
    ty_t *base_ty = find_ptr_base(arg->ty);
    if (!base_ty)
        err("Dereference of non-pointer type %T", arg->ty);

    expr_t *expr = alloc_expr(EXPR_DREF);
    expr->ty = base_ty;
    expr->as_unary.arg = arg;
    return expr;
}

expr_t *make_pos_expr(expr_t *arg)
{
    if (!(arg->ty->kind & TY_ARITH_MASK))
        err("Unary + applied to non-arithmetic type %T", arg->ty);

    if (arg->ty->kind & TY_PROMOTE_MASK)
        arg = make_cast_expr(&ty_int, arg);

    return arg;
}

expr_t *make_neg_expr(expr_t *arg)
{
    if (!(arg->ty->kind & TY_ARITH_MASK))
        err("Unary - applied to non-arithmetic type %T", arg->ty);

    if (arg->ty->kind & TY_PROMOTE_MASK)
        arg = make_cast_expr(&ty_int, arg);

    if (arg->kind == EXPR_CONST) {
        arg->as_const.value = -arg->as_const.value;
        return arg;
    }

    expr_t *expr = alloc_expr(EXPR_NEG);
    expr->ty = arg->ty;
    expr->as_unary.arg = arg;
    return expr;
}

expr_t *make_not_expr(expr_t *arg)
{
    if (!(arg->ty->kind & TY_INT_MASK))
        err("Unary ~ applied to non-integer type %T", arg->ty);

    if (arg->ty->kind & TY_PROMOTE_MASK)
        arg = make_cast_expr(&ty_int, arg);

    if (arg->kind == EXPR_CONST) {
        arg->as_const.value = ~arg->as_const.value;
        return arg;
    }

    expr_t *expr = alloc_expr(EXPR_NEG);
    expr->ty = arg->ty;
    expr->as_unary.arg = arg;
    return expr;
}

expr_t *make_lnot_expr(expr_t *arg)
{
    if (!(arg->ty->kind & TY_SCALAR_MASK))
        err("Unary ! applied to non-scalar type %T", arg->ty);

    if (arg->kind == EXPR_CONST) {
        arg->ty = &ty_int;
        arg->as_const.value = !arg->as_const.value;
        return arg;
    }

    expr_t *expr = alloc_expr(EXPR_LNOT);
    expr->ty = &ty_int;
    expr->as_unary.arg = arg;
    return expr;
}

expr_t *make_sizeof_expr(ty_t *ty)
{
    if (!ty->size)
        err("Requsted size of incomplete type %T", ty);

    return make_const_expr(&ty_ulong, ty->size);
}

expr_t *make_cast_expr(ty_t *ty, expr_t *arg)
{
    // Validate if the cast is allowed
    switch (ty->kind) {
    case TY_VOID:           // From all types
        break;

    case TY_BOOL:           // From any scalar type
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_SHORT:
    case TY_USHORT:
    case TY_INT:
    case TY_UINT:
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
        if (arg->ty->kind & TY_SCALAR_MASK)
            break;

        goto error;

    case TY_FLOAT:          // From arithmetic types
    case TY_DOUBLE:
    case TY_LDOUBLE:
        if (arg->ty->kind & TY_ARITH_MASK)
            break;

        goto error;

    case TY_POINTER:        // From integer and pointer types
        if (arg->ty->kind & TY_INT_MASK)
            break;
        if (find_ptr_base(arg->ty))
            break;

        FALLTHROUGH;

    default:
    error:
        err("Cannot cast %T to %T", arg->ty, ty);
    }

    // Fold cast of constants into the operand
    if (arg->kind == EXPR_CONST) {
        arg->ty = ty;
        return arg;
    }

    expr_t *expr = alloc_expr(EXPR_CAST);
    expr->ty = ty;
    expr->as_unary.arg = arg;
    return expr;
}

//
// Infix expressions
//

void binary_promote(expr_t **lhs, expr_t **rhs)
{
    ty_t *ty;
    int kind1 = (*lhs)->ty->kind,
        kind2 = (*rhs)->ty->kind;

    // Find the common arithmetic type
    if (kind1 == TY_LDOUBLE || kind2 == TY_LDOUBLE)
        ty = &ty_ldouble;
    else if (kind1 == TY_DOUBLE || kind2 == TY_DOUBLE)
        ty = &ty_double;
    else if (kind1 == TY_FLOAT || kind2 == TY_FLOAT)
        ty = &ty_float;
    else if (kind1 == TY_ULLONG || kind2 == TY_ULLONG)
        ty = &ty_ullong;
    else if (kind1 == TY_ULONG || kind2 == TY_ULONG)
        ty = &ty_ulong;
    else if (kind1 == TY_LLONG || kind2 == TY_LLONG)
        ty = &ty_llong;
    else if (kind1 == TY_LONG || kind2 == TY_LONG)
        ty = &ty_long;
    else if (kind1 == TY_UINT || kind2 == TY_UINT)
        ty = &ty_uint;
    else
        ty = &ty_int;

    // Then perform any promotions that might be required
    if (kind1 != ty->kind)
        *lhs = make_cast_expr(ty, *lhs);
    if (kind2 != ty->kind)
        *rhs = make_cast_expr(ty, *rhs);
}

expr_t *make_mul_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK)
        goto type_ok;

    err("Cannot apply * to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value *= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_MUL);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_div_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK)
        goto type_ok;

    err("Cannot apply / to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value /= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_DIV);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_mod_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_INT_MASK && rhs->ty->kind & TY_INT_MASK)
        goto type_ok;

    err("Cannot apply %% to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value %= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_MOD);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

static expr_t *make_ptraddsub(int kind, ty_t *base_ty, expr_t *ptr, expr_t *idx)
{
    if (!(idx->ty->kind & TY_INT_MASK))
        err("Non-integer type %T used as a pointer offset", idx->ty);

    expr_t *expr = alloc_expr(kind);
    expr->ty = make_pointer(base_ty);
    expr->as_binary.lhs = ptr;
    expr->as_binary.rhs = make_mul_expr(idx, make_sizeof_expr(base_ty));
    return expr;
}

expr_t *make_add_expr(expr_t *lhs, expr_t *rhs)
{
    ty_t *base_ty;
    if ((base_ty = find_ptr_base(lhs->ty)))
        return make_ptraddsub(EXPR_ADD, base_ty, lhs, rhs);
    if ((base_ty = find_ptr_base(rhs->ty)))
        return make_ptraddsub(EXPR_ADD, base_ty, rhs, lhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK)
        goto type_ok;

    err("Cannot apply + to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value += rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_ADD);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_sub_expr(expr_t *lhs, expr_t *rhs)
{
    ty_t *base_ty;

    if ((base_ty = compare_ptrs(lhs->ty, rhs->ty, false))) {
        expr_t *expr = alloc_expr(EXPR_SUB);
        expr->ty = &ty_long;
        expr->as_binary.lhs = lhs;
        expr->as_binary.rhs = rhs;
        return make_div_expr(expr, make_sizeof_expr(base_ty));
    }

    if ((base_ty = find_ptr_base(lhs->ty)))
        return make_ptraddsub(EXPR_SUB, base_ty, lhs, rhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK)
        goto type_ok;

    err("Cannot apply - to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value -= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_SUB);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_lsh_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_INT_MASK && rhs->ty->kind & TY_INT_MASK)
        goto type_ok;

    err("Cannot apply << to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->ty->kind & TY_PROMOTE_MASK)
        lhs = make_cast_expr(&ty_int, lhs);
    if (rhs->ty->kind & TY_PROMOTE_MASK)
        lhs = make_cast_expr(&ty_int, rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value <<= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_LSH);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_rsh_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_INT_MASK && rhs->ty->kind & TY_INT_MASK)
        goto type_ok;

    err("Cannot apply >> to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->ty->kind & TY_PROMOTE_MASK)
        lhs = make_cast_expr(&ty_int, lhs);
    if (rhs->ty->kind & TY_PROMOTE_MASK)
        lhs = make_cast_expr(&ty_int, rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value >>= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_RSH);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_lt_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (compare_ptrs(lhs->ty, rhs->ty, false))
        goto type_ok;

    err("Cannot apply < to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value < rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_LT);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_gt_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (compare_ptrs(lhs->ty, rhs->ty, false))
        goto type_ok;

    err("Cannot apply > to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value > rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_GT);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_le_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (compare_ptrs(lhs->ty, rhs->ty, false))
        goto type_ok;

    err("Cannot apply <= to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value <= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_LE);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_ge_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (compare_ptrs(lhs->ty, rhs->ty, false))
        goto type_ok;

    err("Cannot apply >= to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value >= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_GE);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_eq_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (compare_ptrs(lhs->ty, rhs->ty, true))
        goto type_ok;


    if (find_ptr_base(lhs->ty) && is_null_const(rhs)) {
        rhs = make_cast_expr(lhs->ty, rhs);
        goto type_ok;
    }

    if (find_ptr_base(rhs->ty) && is_null_const(lhs)) {
        lhs = make_cast_expr(rhs->ty, lhs);
        goto type_ok;
    }

    err("Cannot apply == to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value == rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_EQ);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_ne_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (compare_ptrs(lhs->ty, rhs->ty, true))
        goto type_ok;

    if (find_ptr_base(lhs->ty) && is_null_const(rhs)) {
        rhs = make_cast_expr(lhs->ty, rhs);
        goto type_ok;
    }

    if (find_ptr_base(rhs->ty) && is_null_const(lhs)) {
        lhs = make_cast_expr(rhs->ty, lhs);
        goto type_ok;
    }

    err("Cannot apply != to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value != rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_NE);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_and_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_INT_MASK && rhs->ty->kind & TY_INT_MASK)
        goto type_ok;

    err("Cannot apply | to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value &= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_AND);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_xor_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_INT_MASK && rhs->ty->kind & TY_INT_MASK)
        goto type_ok;

    err("Cannot apply ^ to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value ^= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_XOR);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_or_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_INT_MASK && rhs->ty->kind & TY_INT_MASK)
        goto type_ok;

    err("Cannot apply | to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    binary_promote(&lhs, &rhs);

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value |= rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_OR);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_land_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_SCALAR_MASK && rhs->ty->kind & TY_SCALAR_MASK)
        goto type_ok;

    err("Cannot apply && to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value
                            && rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_LAND);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_lor_expr(expr_t *lhs, expr_t *rhs)
{
    if (lhs->ty->kind & TY_SCALAR_MASK && rhs->ty->kind & TY_SCALAR_MASK)
        goto type_ok;

    err("Cannot apply || to types %T and %T", lhs->ty, rhs->ty);

type_ok:

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST) {
        lhs->as_const.value = lhs->as_const.value
                            || rhs->as_const.value;
        return lhs;
    }

    expr_t *expr = alloc_expr(EXPR_LOR);
    expr->ty = &ty_int;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_cond_expr(expr_t *cond, expr_t *val1, expr_t *val2)
{
    if (!(cond->ty->kind & TY_SCALAR_MASK))
        err("Non-scalar first operand %T of ?:", cond->ty);

    if (val1->ty->kind & TY_ARITH_MASK && val2->ty->kind & TY_ARITH_MASK) {
        binary_promote(&val1, &val2);
        goto type_ok;
    }

    if (val1->ty->kind & (TY_STRUCT | TY_UNION) && val1->ty == val2->ty)
        goto type_ok;

    if (val1->ty->kind == TY_VOID && val2->ty->kind == TY_VOID)
        goto type_ok;

    if (compare_ptrs(val1->ty, val2->ty, true))
        goto type_ok;

    if (find_ptr_base(val1->ty) && is_null_const(val2)) {
        val2 = make_cast_expr(val1->ty, val2);
        goto type_ok;
    }

    if (find_ptr_base(val2->ty) && is_null_const(val1)) {
        val1 = make_cast_expr(val2->ty, val1);
        goto type_ok;
    }

    err("Incompatible operands %T and %T of ?:", val1->ty, val2->ty);

type_ok:

    if (cond->kind == EXPR_CONST)
        return cond->as_const.value ? val1 : val2;

    expr_t *expr = alloc_expr(EXPR_COND);
    expr->ty = val1->ty;
    expr->as_trinary.arg1 = cond;
    expr->as_trinary.arg2 = val1;
    expr->as_trinary.arg3 = val2;
    return expr;
}

expr_t *make_as_expr(expr_t *lhs, expr_t *rhs)
{
    expr_t *expr = alloc_expr(EXPR_AS);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = convert_by_assignment(lhs->ty, rhs);
    return expr;
}

expr_t *make_seq_expr(expr_t *lhs, expr_t *rhs)
{
    expr_t *expr = alloc_expr(EXPR_SEQ);
    expr->ty = rhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_stmt_expr(stmt_vec_t *stmts)
{
    ty_t *ty = &ty_void;
    if (stmts->length > 0) {
        stmt_t *stmt = VEC_AT(stmts, stmts->length - 1);
        if (stmt->kind == STMT_EVAL)
            ty = stmt->as_eval.value->ty;
    }

    expr_t *expr = alloc_expr(EXPR_STMT);
    expr->ty = ty;
    expr->as_stmts = *stmts;
    return expr;
}

void make_init_expr(init_t *out, ty_t *dest_ty, expr_t *expr)
{
    out->kind = INIT_EXPR;
    out->as_expr = convert_by_assignment(dest_ty, expr);
}

void make_init_list(init_t *out, init_vec_t *list)
{
    out->kind = INIT_LIST;
    out->as_list = *list;
}

stmt_t *make_stmt(int kind)
{
    stmt_t *stmt = calloc(1, sizeof *stmt);
    if (!stmt) abort();
    stmt->kind = kind;
    return stmt;
}
