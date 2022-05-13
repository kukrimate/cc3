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
ty_t ty_ldouble = { TY_LDOUBLE, 16,     16,     };

memb_t *make_memb(ty_t *ty, char *name)
{
    memb_t *memb = calloc(1, sizeof *memb);
    if (!memb) abort();
    memb->ty = ty;
    memb->name = name;
    return memb;
}

void define_struct(ty_t *ty, memb_t *members)
{
    if (ty->as_aggregate.members)
        err("Re-definition of struct");

    ty->as_aggregate.members = members;

    for (memb_t *memb = members; memb; memb = memb->next) {
        // The highest member alignment becomes the struct's alignment
        if (memb->ty->align > ty->align)
            ty->align = memb->ty->align;
        // Then we align the current size to the member's alignment
        // and that becomes the member's offset
        memb->offset = (ty->size = align(ty->size, memb->ty->align));
        // Finally we increase the size by the member's size
        ty->size += memb->ty->size;
    }
    // The struct's size must be aligned to be a multiple f it's alignment
    ty->size = align(ty->size, ty->align);
}

void define_union(ty_t *ty, memb_t *members)
{
    if (ty->as_aggregate.members)
        err("Re-definition of union");

    ty->as_aggregate.members = members;

    for (memb_t *memb = members; memb; memb = memb->next) {
        // The highest member alignment becomes the union's alignment
        if (memb->ty->align > ty->align)
            ty->align = memb->ty->align;
        // The highest member size becomes the union's size
        if (memb->ty->size > ty->size)
            ty->size = memb->ty->size;
    }
}

param_t *make_param(ty_t *ty, sym_t *sym)
{
    param_t *param = calloc(1, sizeof *param);
    if (!param) abort();
    param->ty = ty;
    param->sym = sym;
    return param;
}

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
    if (elem_ty->kind == TY_STRUCT || elem_ty->kind == TY_STRUCT)
        if (!elem_ty->as_aggregate.members)
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

ty_t *make_function(ty_t *ret_ty, scope_t *scope, param_t *params, bool var)
{
    if (ret_ty->kind == TY_ARRAY)
        err("Function returning array %T", ret_ty);
    if (ret_ty->kind == TY_STRUCT || ret_ty->kind == TY_STRUCT)
        if (!ret_ty->as_aggregate.members)
            err("Function returning incomplete aggregate %T", ret_ty);
    if (ret_ty->kind == TY_FUNCTION)
        err("Function returning function %T", ret_ty);

    ty_t *ty = make_ty(TY_FUNCTION);
    ty->function.ret_ty = ret_ty;
    ty->function.scope = scope;
    ty->function.params = params;
    ty->function.var = var;
    return ty;
}

bool compare_ty(ty_t *ty1, ty_t *ty2, bool permit_void_ptr)
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

        // In some contexts void* compares equal to any pointer
        if (permit_void_ptr) {
            if (ty1->pointer.base_ty->kind == TY_VOID)
                return true;
            if (ty2->pointer.base_ty->kind == TY_VOID)
                return true;
        }

        return compare_ty(ty1->pointer.base_ty, ty2->pointer.base_ty, false);

    case TY_ARRAY:
        if (ty2->kind != TY_ARRAY)
            return false;
        if (ty1->array.cnt != ty2->array.cnt)
            return false;
        return compare_ty(ty1->array.elem_ty, ty2->array.elem_ty, false);

    case TY_FUNCTION:
        if (ty2->kind != TY_FUNCTION)
            return false;
        if (ty1->function.var != ty2->function.var)
            return false;
        if (!compare_ty(ty1->function.ret_ty, ty2->function.ret_ty, false))
            return false;
        param_t *p1 = ty1->function.params, *p2 = ty2->function.params;
        for (; p1 && p2; p1 = p1->next, p2 = p2->next)
            if (!compare_ty(p1->ty, p2->ty, false))
                return false;
        return !p1 && !p2;

    default:
        ASSERT_NOT_REACHED();
    }
}

void sema_init(sema_t *self)
{
    self->scope = NULL;
    sema_enter(self);

    self->block_static_cnt = 0;

    // Declare va_list type, this is defined by the SysV AMD64 ABI

    // typedef struct {
    //     unsigned int gp_offset;
    //     unsigned int fp_offset;
    //     void *overflow_arg_area;
    //     void *reg_save_area;
    // } va_list[1];

    ty_t *va_list_struct = make_ty(TY_STRUCT);
    memb_t *memb, **tail = &memb;
    *tail = make_memb(&ty_uint, "gp_offset");
    tail = &(*tail)->next;
    *tail = make_memb(&ty_uint, "fp_offset");
    tail = &(*tail)->next;
    *tail = make_memb(make_pointer(&ty_void), "overflow_arg_area");
    tail = &(*tail)->next;
    *tail = make_memb(make_pointer(&ty_void), "reg_save_area");
    define_struct(va_list_struct, memb);

    sema_declare(self, TK_TYPEDEF,
        make_array(va_list_struct, 1),
        strdup("__builtin_va_list"));
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

sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, char *name)
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

void sema_alloc_local(sema_t *self, sym_t *sym)
{
    assert(sym->kind == SYM_LOCAL);

    // First align the stack
    self->offset = align(self->offset, sym->ty->align);
    // Then we can allocate space for the variable
    sym->offset = self->offset;
    self->offset += sym->ty->size;
}

sym_t *sema_declare_enum_const(sema_t *self, char *name, val_t val)
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
    ty->as_aggregate.name = strdup(name);

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
    ty->as_aggregate.name = strdup(name);
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
        return make_str_expr(strdup(self->func_name));

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

expr_t *make_str_expr(char *data)
{
    expr_t *expr = alloc_expr(EXPR_STR);
    expr->ty = make_array(&ty_char, strlen(data) + 1);
    expr->as_str.data = data;
    return expr;
}

// Degrade array and function types to pointers per (6.3.2.1)

static expr_t *pointer_conv(expr_t *arg)
{
    if (arg->ty->kind == TY_ARRAY) {
        expr_t *expr = alloc_expr(EXPR_REF);
        expr->ty = make_pointer(arg->ty->array.elem_ty);
        expr->as_unary.arg = arg;
        return expr;
    }

    if (arg->ty->kind == TY_FUNCTION)
        return make_ref_expr(arg);

    return arg;
}

//
// Postfix expressions
//

static int find_memb(ty_t *ty, const char *name, ty_t **out)
{
    if (ty->kind != TY_STRUCT && ty->kind != TY_UNION)
        return -1;

    for (memb_t *memb = ty->as_aggregate.members; memb; memb = memb->next) {
        if (!memb->name) {
            int result = find_memb(memb->ty, name, out);
            if (result != -1) {
                return memb->offset + result;
            }
        } else if (!strcmp(memb->name, name)) {
            *out = memb->ty;
            return memb->offset;
        }
    }

    return -1;
}

expr_t *make_memb_expr(expr_t *aggr, const char *name)
{
    aggr = pointer_conv(aggr);

    expr_t *expr = alloc_expr(EXPR_MEMB);
    expr->as_memb.aggr = aggr;
    expr->as_memb.name = strdup(name);
    expr->as_memb.offset = find_memb(aggr->ty, name, &expr->ty);
    if (expr->as_memb.offset == -1)
        err("Non-existent member %s of type %T", name, aggr->ty);
    return expr;
}

static expr_t *convert_by_assignment(ty_t *dest_ty, expr_t *src);

expr_t *make_call_expr(expr_t *func, expr_vec_t *args)
{
    func = pointer_conv(func);

    // Called expression must be a function pointer after implicit conversions
    ty_t *func_ty = NULL;
    if (func->ty->kind == TY_POINTER && func->ty->pointer.base_ty->kind == TY_FUNCTION)
        func_ty = func->ty->pointer.base_ty;
    if (!func_ty)
        err("Cannot call %T object", func->ty);


    // Convert the arguments to the type of the corresponding parameters
    param_t *param = func_ty->function.params;
    VEC_FOREACH(args, arg)
        if (param) {
            // Function arguments get converted "as if by assignment"
            *arg = convert_by_assignment(param->ty, pointer_conv(*arg));
            param = param->next;
        } else if (func_ty->function.var) {
            // Variable arguments get the "default argument promotions"
            if ((*arg)->ty->kind & TY_PROMOTE_MASK)
                *arg = make_cast_expr(&ty_int, *arg);
            else if ((*arg)->ty->kind == TY_FLOAT)
                *arg = make_cast_expr(&ty_double, *arg);
        } else {
            err("Too many arguments for %T", func_ty);
        }
    if (param)
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
    arg = pointer_conv(arg);

    if (arg->ty->kind != TY_POINTER)
        err("Dereference of non-pointer type %T", arg->ty);

    expr_t *expr = alloc_expr(EXPR_DREF);
    expr->ty = arg->ty->pointer.base_ty;
    expr->as_unary.arg = arg;
    return expr;
}

expr_t *make_pos_expr(expr_t *arg)
{
    arg = pointer_conv(arg);

    if (!(arg->ty->kind & TY_ARITH_MASK))
        err("Unary + applied to non-arithmetic type %T", arg->ty);

    if (arg->ty->kind & TY_PROMOTE_MASK)
        arg = make_cast_expr(&ty_int, arg);

    return arg;
}

expr_t *make_neg_expr(expr_t *arg)
{
    arg = pointer_conv(arg);

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
    arg = pointer_conv(arg);

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
    arg = pointer_conv(arg);

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
    if (ty->size == 0)
        err("Requsted size of incomplete type %T", ty);

    return make_const_expr(&ty_ulong, ty->size);
}

expr_t *make_cast_expr(ty_t *ty, expr_t *arg)
{
    arg = pointer_conv(arg);

    // Avoid redundant type conversions
    if (compare_ty(ty, arg->ty, false))
        return arg;

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
        if (arg->ty->kind & (TY_INT_MASK | TY_POINTER))
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
    int kind1 = (*lhs)->ty->kind, kind2 = (*rhs)->ty->kind;

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
    *lhs = make_cast_expr(ty, *lhs);
    *rhs = make_cast_expr(ty, *rhs);
}

expr_t *make_mul_expr(expr_t *lhs, expr_t *rhs)
{
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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

static expr_t *make_ptraddsub(int kind, expr_t *ptr, expr_t *idx)
{
    if (!(idx->ty->kind & TY_INT_MASK))
        err("Non-integer type %T used as a pointer offset", idx->ty);

    expr_t *expr = alloc_expr(kind);
    expr->ty = ptr->ty;
    expr->as_binary.lhs = ptr;
    expr->as_binary.rhs = make_mul_expr(idx,
        make_sizeof_expr(ptr->ty->pointer.base_ty));
    return expr;
}

expr_t *make_add_expr(expr_t *lhs, expr_t *rhs)
{
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind == TY_POINTER)
        return make_ptraddsub(EXPR_ADD, lhs, rhs);
    if (rhs->ty->kind == TY_POINTER)
        return make_ptraddsub(EXPR_ADD, rhs, lhs);

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

static expr_t *make_ptrdiff(expr_t *ptr1, expr_t *ptr2)
{
    if (!compare_ty(ptr1->ty, ptr2->ty, false))
        err("Substracted different pointers %T and %T", ptr1->ty, ptr2->ty);

    expr_t *expr = alloc_expr(EXPR_SUB);
    expr->ty = &ty_long;
    expr->as_binary.lhs = ptr1;
    expr->as_binary.rhs = ptr2;
    return make_div_expr(expr, make_sizeof_expr(ptr1->ty->pointer.base_ty));
}

expr_t *make_sub_expr(expr_t *lhs, expr_t *rhs)
{
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind == TY_POINTER) {
        if (rhs->ty->kind == TY_POINTER)
            return make_ptrdiff(lhs, rhs);
        else
            return make_ptraddsub(EXPR_SUB, lhs, rhs);
    }

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (lhs->ty->kind == TY_POINTER && compare_ty(lhs->ty, rhs->ty, false))
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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (lhs->ty->kind == TY_POINTER && compare_ty(lhs->ty, rhs->ty, false))
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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (lhs->ty->kind == TY_POINTER && compare_ty(lhs->ty, rhs->ty, false))
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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (lhs->ty->kind == TY_POINTER && compare_ty(lhs->ty, rhs->ty, false))
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

static bool is_null_const(expr_t *expr)
{
    return expr->ty->kind & TY_INT_MASK &&
            expr->kind == EXPR_CONST &&
            expr->as_const.value == 0;
}

expr_t *make_eq_expr(expr_t *lhs, expr_t *rhs)
{
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (lhs->ty->kind == TY_POINTER && compare_ty(lhs->ty, rhs->ty, true))
        goto type_ok;

    if (lhs->ty->kind == TY_POINTER && is_null_const(rhs)) {
        rhs = make_cast_expr(lhs->ty, rhs);
        goto type_ok;
    }

    if (is_null_const(lhs) && rhs->ty->kind == TY_POINTER) {
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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    if (lhs->ty->kind & TY_ARITH_MASK && rhs->ty->kind & TY_ARITH_MASK) {
        binary_promote(&lhs, &rhs);
        goto type_ok;
    }

    if (lhs->ty->kind == TY_POINTER && compare_ty(lhs->ty, rhs->ty, true))
        goto type_ok;

    if (lhs->ty->kind == TY_POINTER && is_null_const(rhs)) {
        rhs = make_cast_expr(lhs->ty, rhs);
        goto type_ok;
    }

    if (is_null_const(lhs) && rhs->ty->kind == TY_POINTER) {
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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

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
    cond = pointer_conv(cond);
    val1 = pointer_conv(val1);
    val2 = pointer_conv(val2);

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

    if (val1->ty->kind == TY_POINTER && compare_ty(val1->ty, val2->ty, true))
        goto type_ok;

    if (val1->ty->kind == TY_POINTER && is_null_const(val2)) {
        val2 = make_cast_expr(val1->ty, val2);
        goto type_ok;
    }

    if (is_null_const(val1) && val2->ty->kind == TY_POINTER) {
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

static expr_t *convert_by_assignment(ty_t *dest_ty, expr_t *src)
{
    if (dest_ty->kind & TY_ARITH_MASK && src->ty->kind & TY_ARITH_MASK)
        goto type_ok;

    if (dest_ty->kind & (TY_STRUCT | TY_UNION) && dest_ty == src->ty)
        goto type_ok;

    if (dest_ty->kind == TY_POINTER && compare_ty(dest_ty, src->ty, true))
        goto type_ok;

    if (dest_ty->kind == TY_POINTER && is_null_const(src))
        goto type_ok;

    if (dest_ty->kind == TY_BOOL && src->ty->kind == TY_POINTER)
        goto type_ok;

    err("Cannot convert type %T to %T by assignment", src->ty, dest_ty);

type_ok:

    return make_cast_expr(dest_ty, src);
}

expr_t *make_as_expr(expr_t *lhs, expr_t *rhs)
{
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    expr_t *expr = alloc_expr(EXPR_AS);
    expr->ty = lhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = convert_by_assignment(lhs->ty, rhs);
    return expr;
}

expr_t *make_seq_expr(expr_t *lhs, expr_t *rhs)
{
    lhs = pointer_conv(lhs);
    rhs = pointer_conv(rhs);

    expr_t *expr = alloc_expr(EXPR_SEQ);
    expr->ty = rhs->ty;
    expr->as_binary.lhs = lhs;
    expr->as_binary.rhs = rhs;
    return expr;
}

expr_t *make_stmt_expr(stmt_t *body)
{
    expr_t *expr = alloc_expr(EXPR_STMT);

    // Get the type of the last eval (or void)
    stmt_t *last = body;
    while (last->next)
        last = last->next;

    if (last && last->kind == STMT_EVAL)
        expr->ty = last->as_eval.value->ty;
    else
        expr->ty = &ty_void;

    expr->as_stmt = body;
    return expr;
}

static init_t *alloc_init(void)
{
    init_t *init = calloc(1, sizeof *init);
    if (!init) abort();
    return init;
}

init_t *make_init_expr(expr_t *expr)
{
    init_t *init = alloc_init();
    init->kind = INIT_EXPR;
    init->as_expr = expr;
    return init;
}

init_t *make_init_list(init_t *list)
{
    init_t *init = alloc_init();
    init->kind = INIT_LIST;
    init->as_list = list;
    return init;
}

/** This section deals with the suckiness of C initializers so the code
 *  generator doesn't have to. If you need any proof the C standard is downright
 *  insane, filled with arcane special cases, just read this code. **/

static init_t *bind_scalar(init_t **iter)
{
    // If we reach a member of an aggregate without an initializer, we need to
    // zero initialize it. We make that explicit to simplify code generation.
    if (!*iter)
        return make_init_expr(make_const_expr(&ty_int, 0));

    // Otherwise we consume the first initializer from the iterator.
    init_t *init = *iter;
    *iter = (*iter)->next;

    // Then we remove any excess braces that might be present around it.
    while (init->kind == INIT_LIST) {
        // For each layer of braces, the initializer becomes the contents
        init = init->as_list;
        // We also make sure there is no trailing garbage in any layer
        if (init->next)
            err("Trailing garbage in scalar initializer");
    }

    return make_init_expr(init->as_expr);
}

static init_t *bind_init_r(ty_t *ty, init_t **iter, bool outer);

static init_t *bind_struct(ty_t *ty, init_t **iter)
{
    // Build new initializer list
    init_t *head = NULL, **tail = &head;

    // Bind an initializer to each member of the struct
    for (memb_t *memb = ty->as_aggregate.members; memb; memb = memb->next) {
        *tail = bind_init_r(memb->ty, iter, false);
        tail = &(*tail)->next;
    }

    // Return the new initializer list we've built
    return make_init_list(head);
}

static init_t *bind_union(ty_t *ty, init_t **iter)
{
    // Like the struct initializer above, however only the first member
    // can be initialized of an union
    return make_init_list(bind_init_r(ty->as_aggregate.members->ty, iter, false));
}

static init_t *bind_array_str(ty_t *ty, init_t **iter)
{
    switch (ty->array.elem_ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:  // Only applies to char arrays
        break;
    default:        // Bail on any other type
        goto bail;
    }

    if (!*iter)     // Or bail if there is no initializer
        goto bail;

    // Find an expression that might be enclosed in an arbitrary number of braces
    init_t *init = *iter;
    while (init->kind == INIT_LIST) {
        // Look at the contents of the list
        init = init->as_list;
        // Bail if there is anything trailing
        if (init->next)
            goto bail;
    }

    // Bail if it's not a string literal
    if (init->as_expr->kind != EXPR_STR)
        goto bail;

    // The string literal might complete the array's type
    if (ty->array.cnt == -1) {
        int cnt = strlen(init->as_expr->as_str.data) + 1;
        ty->align = ty->array.elem_ty->align;
        ty->size = ty->array.elem_ty->size * cnt;
        ty->array.cnt = cnt;
    }

    return make_init_expr(init->as_expr);

bail:
    return NULL;
}

static init_t *bind_array(ty_t *ty, init_t **iter)
{
    // Otherwise build a new initializer list
    init_t *head = NULL, **tail = &head;

    if (ty->array.cnt == -1) {
        // Bind as many initializers as we can, and count them
        int cnt;
        for (cnt = 0; *iter; ++cnt) {
            *tail = bind_init_r(ty->array.elem_ty, iter, false);
            tail = &(*tail)->next;
        }
        // Then complete the array type with the number of entries
        ty->align = ty->array.elem_ty->align;
        ty->size = ty->array.elem_ty->size * cnt;
        ty->array.cnt = cnt;
    } else {
        // Bind an initializer to each array element
        for (int i = 0; i < ty->array.cnt; ++i) {
            *tail = bind_init_r(ty->array.elem_ty, iter, false);
            tail = &(*tail)->next;
        }
    }
    // Return the new initializer list we've built
    return make_init_list(head);
}

static init_t *bind_init_r(ty_t *ty, init_t **iter, bool outer)
{
    switch (ty->kind) {
    // Struct:
    // - a brace enclosed initializer list
    // - consume elements from the outer initializer list
    case TY_STRUCT:
        if (*iter && (*iter)->kind == INIT_LIST) {
            // Consume elements from the initializer list
            init_t *cur = (*iter)->as_list;
            init_t *init = bind_struct(ty, &cur);
            if (cur)
                err("Trailing garbage in struct initializer");
            // Skip the initializer list we've consumed
            *iter = (*iter)->next;
            // Return new list
            return init;
        }

        // Otherwise consume elements from the outer initializer list
        if (outer)
            err("Invalid initializer for type %T", ty);
        return bind_struct(ty, iter);

    // Union:
    // - a brace enclosed initializer list
    // - consume elements from the outer initializer list
    case TY_UNION:
        if (*iter && (*iter)->kind == INIT_LIST) {
            // Consume elements from the initializer list
            init_t *cur = (*iter)->as_list;
            init_t *init = bind_union(ty, &cur);
            if (cur)
                err("Trailing garbage in union initializer");
            // Only skip the initializer list we've consumed
            *iter = (*iter)->next;
            return init;
        }

        // Otherwise consume elements from the outer initializer list
        if (outer)
            err("Invalid initializer for type %T", ty);
        return bind_union(ty, iter);

    // Array:
    // - for char arrays, a string literal, optionally in braces
    // - a brace enclosed initializer list
    // - consume elements from the outer initializer list
    case TY_ARRAY:
        if (!outer && ty->array.cnt == -1)
            err("Only the outermost array type is allowed to be incomplete");

        // First see if we found a string literal initializer
        init_t *init = bind_array_str(ty, iter);
        if (init)
            return init;

        if (*iter && (*iter)->kind == INIT_LIST) {
            // Consume elements from the initializer list
            init_t *cur = (*iter)->as_list;
            init_t *init = bind_array(ty, &cur);
            if (cur)
                err("Trailing garbage in array initializer");
            // Only skip the initializer list we've consumed
            *iter = (*iter)->next;
            return init;
        }

        // Otherwise consume elements from the outer initializer list
        if (outer)
            err("Invalid initializer for type %T", ty);
        return bind_array(ty, iter);

    // Scalar:
    // - an expression, optionally in braces
    default:
        return bind_scalar(iter);
    }
}

init_t *bind_init(ty_t *ty, init_t *init)
{
    return bind_init_r(ty, &init, true);
}

stmt_t *make_stmt(int kind)
{
    stmt_t *stmt = calloc(1, sizeof *stmt);
    if (!stmt) abort();
    stmt->kind = kind;
    return stmt;
}
