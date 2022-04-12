#include "cc3.h"

ty_t *make_ty(int kind)
{
    ty_t *ty = calloc(1, sizeof *ty);
    ty->rc = 1;
    ty->kind = kind;
    return ty;
}

ty_t *make_pointer(ty_t *base_ty)
{
    ty_t *ty = calloc(1, sizeof *ty);
    ty->rc = 1;
    ty->kind = TY_POINTER;
    ty->pointer.base_ty = base_ty;
    return ty;
}

ty_t *make_array(ty_t *elem_ty, int cnt)
{
    ty_t *ty = calloc(1, sizeof *ty);
    ty->rc = 1;
    ty->kind = TY_ARRAY;
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
    ty_t *ty = calloc(1, sizeof *ty);
    ty->rc = 1;
    ty->kind = TY_FUNCTION;
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

void sema_declare(sema_t *self, int sc, ty_t *ty, char *name)
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

    printf("Declare %s as ", name);
    print_ty(ty);
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
