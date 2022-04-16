#include "cc3.h"

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
    ty->pointer.base_ty = base_ty;
    return ty;
}

ty_t *make_array(ty_t *elem_ty, int cnt)
{
    ty_t *ty = make_ty(TY_ARRAY);
    ty->array.elem_ty = elem_ty;
    ty->array.cnt = cnt;
    return ty;
}

ty_t *make_param(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID:
        err("Parameter type cannot be void");
    case TY_ARRAY: {
        ty_t *elem_ty = ty->array.elem_ty;
        free(ty);
        return make_pointer(elem_ty);
    }
    case TY_FUNCTION:
        return make_pointer(ty);
    default:
        return ty;
    }
}

ty_t *make_function(ty_t *ret_ty, ty_t *param_tys, bool var)
{
    ty_t *ty = make_ty(TY_FUNCTION);
    if (ret_ty->kind == TY_ARRAY)
        err("Function returning array");
    if (ret_ty->kind == TY_FUNCTION)
        err("Function returning function");
    ty->function.ret_ty = ret_ty;
    ty->function.param_tys = param_tys;
    ty->function.var = var;
    return ty;
}

ty_t *clone_ty(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID:
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
    case TY_BOOL:
        return make_ty(ty->kind);

    case TY_POINTER:
        return make_pointer(clone_ty(ty->pointer.base_ty));

    case TY_ARRAY:
        return make_array(clone_ty(ty->array.elem_ty), ty->array.cnt);

    case TY_FUNCTION: {
        ty_t *param_tys = NULL, **tail = &param_tys;
        for (ty_t *param_ty = ty->function.param_tys; param_ty; param_ty = param_ty->next) {
            *tail = clone_ty(param_ty);
            tail = &(*tail)->next;
        }
        return make_function(clone_ty(ty->function.ret_ty), param_tys, ty->function.var);
    }

    default:
        ASSERT_NOT_REACHED();
    }
}

int ty_align(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR:       return 1;
    case TY_SCHAR:      return 1;
    case TY_UCHAR:      return 1;
    case TY_SHORT:      return 2;
    case TY_USHORT:     return 2;
    case TY_INT:        return 4;
    case TY_UINT:       return 4;
    case TY_LONG:       return 8;
    case TY_ULONG:      return 8;
    case TY_LLONG:      return 8;
    case TY_ULLONG:     return 8;
    case TY_FLOAT:      return 4;
    case TY_DOUBLE:     return 8;
    case TY_LDOUBLE:    return 16;
    case TY_BOOL:       return 1;
    case TY_POINTER:    return 8;
    case TY_ARRAY:
        return ty_align(ty->array.elem_ty);
    default:
        ASSERT_NOT_REACHED();
    }
}

int ty_size(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR:       return 1;
    case TY_SCHAR:      return 1;
    case TY_UCHAR:      return 1;
    case TY_SHORT:      return 2;
    case TY_USHORT:     return 2;
    case TY_INT:        return 4;
    case TY_UINT:       return 4;
    case TY_LONG:       return 8;
    case TY_ULONG:      return 8;
    case TY_LLONG:      return 8;
    case TY_ULLONG:     return 8;
    case TY_FLOAT:      return 4;
    case TY_DOUBLE:     return 8;
    case TY_LDOUBLE:    return 16;
    case TY_BOOL:       return 1;
    case TY_POINTER:    return 8;
    case TY_ARRAY:
        assert(ty->array.cnt != -1);
        return ty_size(ty->array.elem_ty) * ty->array.cnt;
    default:
        ASSERT_NOT_REACHED();
    }
}

void free_ty(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID:
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
    case TY_BOOL:
        break;

    case TY_POINTER:
        free_ty(ty->pointer.base_ty);
        break;

    case TY_ARRAY:
        free_ty(ty->array.elem_ty);
        break;

    case TY_FUNCTION:
        free_ty(ty->function.ret_ty);
        for (ty_t *param_ty = ty->function.param_tys; param_ty; ) {
            ty_t *tmp = param_ty->next;
            free_ty(param_ty);
            param_ty = tmp;
        }
        break;

    default:
        ASSERT_NOT_REACHED();
    }

    free(ty);
}

static void print_ty_r(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID:       printf("void");                 break;
    case TY_CHAR:       printf("char");                 break;
    case TY_SCHAR:      printf("signed char");          break;
    case TY_UCHAR:      printf("unsigned char");        break;
    case TY_SHORT:      printf("short");                break;
    case TY_USHORT:     printf("unsigned short");       break;
    case TY_INT:        printf("int");                  break;
    case TY_UINT:       printf("unsigned int");         break;
    case TY_LONG:       printf("long");                 break;
    case TY_ULONG:      printf("unsigned long");        break;
    case TY_LLONG:      printf("long long");            break;
    case TY_ULLONG:     printf("unsigned long long");   break;
    case TY_FLOAT:      printf("float");                break;
    case TY_DOUBLE:     printf("double");               break;
    case TY_LDOUBLE:    printf("long double");          break;
    case TY_BOOL:       printf("_Bool");                break;

    case TY_POINTER:
        print_ty_r(ty->pointer.base_ty);
        printf("*");
        break;

    case TY_ARRAY:
        print_ty_r(ty->array.elem_ty);
        if (ty->array.cnt < 0)
            printf("[]");
        else
            printf("[%d]", ty->array.cnt);
        break;

    case TY_FUNCTION:
        print_ty_r(ty->function.ret_ty);

        printf("(");
        ty_t *param_ty = ty->function.param_tys;
        while (param_ty) {
            print_ty_r(param_ty);
            if ((param_ty = param_ty->next) || ty->function.var)
                printf(", ");
        }
        if (ty->function.var)
            printf("...");
        printf(")");
        break;

    default:
        ASSERT_NOT_REACHED();
    }
}

void print_ty(ty_t *ty)
{
    print_ty_r(ty);
    printf("\n");
}

void sema_init(sema_t *self)
{
    self->scope = NULL;
    sema_enter(self);
}

void sema_free(sema_t *self)
{
    sema_exit(self);
}

void sema_enter(sema_t *self)
{
    scope_t *scope = calloc(1, sizeof *scope);
    if (!scope) abort();

    // Replace current scope with the new scope
    scope->parent = self->scope;
    self->scope = scope;
}

void sema_exit(sema_t *self)
{
    // Replace current scope with parent
    scope_t *scope = self->scope;
    self->scope = scope->parent;

    // Free symbols
    for (sym_t *sym = scope->syms; sym; ) {
        sym_t *tmp = sym->next;
        free_ty(sym->ty);
        free(sym->name);
        free(sym);
        sym = tmp;
    }

    // Free scope
    free(scope);
}

sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, char *name)
{
    struct sym *sym;

    // Find previous declaration in the current scope
    for (sym = self->scope->syms; sym; sym = sym->next)
        if (strcmp(sym->name, name) == 0)
            break;

    if (sym) {
        // FIXME: allow re-declarations in some cases
        err("Re-declaration of symbol %s", name);
    } else {
        // Otherwise add new symbol to current scope
        sym = calloc(1, sizeof *sym);
        sym->next = self->scope->syms;
        self->scope->syms = sym;
    }

    sym->sc = sc;
    sym->ty = ty;
    sym->name = name;

    if (self->scope->parent) {
        // First align the stack
        self->offset = align(self->offset, ty_align(ty));
        // Then we can allocate space for the variable
        sym->offset = self->offset;
        self->offset += ty_size(ty);
    } else {
        sym->offset = -1;
    }

/*
    printf("Declare %s as ", name);
    print_ty(ty);
*/

    return sym;
}

sym_t *sema_lookup(sema_t *self, const char *name)
{
    for (scope_t *scope = self->scope; scope; scope = scope->parent)
        for (sym_t *sym = scope->syms; sym; sym = sym->next)
            if (strcmp(sym->name, name) == 0)
                return sym;
    return NULL;
}

ty_t *sema_findtypedef(sema_t *self, const char *name)
{
    struct sym *sym = sema_lookup(self, name);
    if (sym && sym->sc == TK_TYPEDEF) {
        return clone_ty(sym->ty);
    }
    return NULL;
}

static expr_t *make_expr(int kind)
{
    expr_t *expr = calloc(1, sizeof *expr);
    if (!expr) abort();
    expr->kind = kind;
    return expr;
}

expr_t *make_sym(sema_t *self, tk_t *tk)
{
    sym_t *sym = sema_lookup(self, tk_str(tk));

    // Find symbol
    if (!sym)
        err("Undeclared identifier %s", tk_str(tk));

    // Create expression based on symbol type
    expr_t *expr;
    if (sym->offset == -1) {
        expr = make_expr(EXPR_GLOBAL);
        expr->str = strdup(sym->name);
    } else {
        expr = make_expr(EXPR_LOCAL);
        expr->offset = sym->offset;
    }
    expr->ty = clone_ty(sym->ty);
    return expr;
}

expr_t *make_const(tk_t *tk)
{
    expr_t *expr = make_expr(EXPR_CONST);
    const char *spelling = tk_str(tk);
    expr->ty = make_ty(TY_INT);
    if (*spelling == '\'') {    // Character constant
        ++spelling;
        while (*spelling != '\'')
            expr->val = expr->val << 8 | unescape(spelling, &spelling);
    } else {                    // Integer constant
        // Decode value
        errno = 0;
        char *end;
        expr->val = strtoull(spelling, &end, 0);
        if (errno || *end)  // FIXME: decode suffix
            err("Invalid integer constant %s", spelling);
    }
    return expr;
}

expr_t *make_str_lit(tk_t *tk)
{
    expr_t *expr = make_expr(EXPR_STR_LIT);
    string_t str;
    string_init(&str);
    const char *spelling = tk_str(tk);
    ++spelling;
    while (*spelling != '\"')
        string_push(&str, unescape(spelling, &spelling));
    expr->ty = make_array(make_ty(TY_CHAR), str.length + 1);
    expr->str = str.data;
    return expr;
}

// Does values of type ty degrade to a pointer type
// and if so what is the base type of said pointer?
static ty_t *pointer_to_what(ty_t *ty)
{
    if (ty->kind == TY_POINTER)           // Actually a pointer
        return ty->pointer.base_ty;
    else if (ty->kind == TY_ARRAY)        // Degrades to pointer
        return ty->array.elem_ty;
    else if (ty->kind == TY_FUNCTION)     // Degrades to pointer
        return ty;
    else
        return NULL;
}

expr_t *make_unary(int kind, expr_t *arg1)
{
    expr_t *expr = make_expr(kind);
    switch (kind) {
    case EXPR_REF:                                  // Non-lvalues will be caught later
        expr->ty = make_pointer(arg1->ty);
        break;
    case EXPR_DREF:
        if (!(expr->ty = pointer_to_what(arg1->ty)))
            err("Pointer required");
        break;
    case EXPR_NEG:
    case EXPR_NOT:
        expr->ty = arg1->ty;
        break;
    case EXPR_LNOT:
        expr->ty = make_ty(TY_INT);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    expr->arg1 = arg1;
    return expr;
}

expr_t *make_binary(int kind, expr_t *arg1, expr_t *arg2)
{
    expr_t *expr = make_expr(kind);

    switch (kind) {
    case EXPR_CALL:
    {
        ty_t *func_ty = pointer_to_what(arg1->ty);
        if (!func_ty || func_ty->kind != TY_FUNCTION)
            err("Non-function object called");
        expr->ty = func_ty->function.ret_ty;
        break;
    }

    case EXPR_ADD:
    case EXPR_SUB:
    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
    case EXPR_LSH:
    case EXPR_RSH:
    case EXPR_AND:
    case EXPR_XOR:
    case EXPR_OR:
        expr->ty = arg1->ty;
        break;

    case EXPR_LT:
    case EXPR_GT:
    case EXPR_LE:
    case EXPR_GE:
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_LAND:
    case EXPR_LOR:
        expr->ty = make_ty(TY_INT);
        break;

    case EXPR_AS:
        expr->ty = arg1->ty;
        break;

    case EXPR_SEQ:
        expr->ty = arg2->ty;
        break;

    default:
        ASSERT_NOT_REACHED();
    }
    expr->arg1 = arg1;
    expr->arg2 = arg2;
    return expr;
}

expr_t *make_trinary(int kind, expr_t *arg1, expr_t *arg2, expr_t *arg3)
{
    expr_t *expr = make_expr(kind);
    assert(kind == EXPR_COND);
    expr->ty = arg2->ty;
    expr->arg1 = arg1;
    expr->arg2 = arg2;
    expr->arg3 = arg3;
    return expr;
}

stmt_t *make_stmt(int kind)
{
    stmt_t *stmt = calloc(1, sizeof *stmt);
    if (!stmt) abort();
    stmt->kind = kind;
    return stmt;
}
