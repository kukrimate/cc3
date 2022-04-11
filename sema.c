#include "cc3.h"

struct ty *make_ty(int kind)
{
    struct ty *ty = calloc(1, sizeof *ty);
    ty->kind = kind;
    return ty;
}

struct ty *make_pointer(struct ty *base_ty)
{
    struct ty *ty = calloc(1, sizeof *ty);
    ty->kind = TY_POINTER;
    ty->pointer.base_ty = base_ty;
    return ty;
}

struct ty *make_array(struct ty *elem_ty, int cnt)
{
    struct ty *ty = calloc(1, sizeof *ty);
    ty->kind = TY_ARRAY;
    ty->array.elem_ty = elem_ty;
    ty->array.cnt = cnt;
    return ty;
}

struct ty *make_function(struct ty *ret_ty, struct ty *param_tys, bool var)
{
    struct ty *ty = calloc(1, sizeof *ty);
    ty->kind = TY_FUNCTION;
    ty->function.ret_ty = ret_ty;
    ty->function.param_tys = param_tys;
    ty->function.var = var;
    return ty;
}

static void print_ty_r(struct ty *ty)
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
        struct ty *param_ty = ty->function.param_tys;
        while (param_ty) {
            print_ty_r(param_ty);
            if ((param_ty = param_ty->next) || ty->function.var)
                printf(", ");
        }
        if (ty->function.var)
            printf("...");
        printf(")");
    }
}

void print_ty(struct ty *ty)
{
    print_ty_r(ty);
    printf("\n");
}

void sema_enter(struct cc3 *self)
{
    struct scope *new_scope = calloc(1, sizeof *new_scope);

    // Replace current scope with the new scope
    new_scope->parent = self->cur_scope;
    self->cur_scope = new_scope;
}

void sema_exit(struct cc3 *self)
{
    struct scope *cur_scope = self->cur_scope;

    // Replace current scope with parent
    assert(cur_scope->parent);
    self->cur_scope = cur_scope->parent;

    // Free symbols
    struct sym *sym = cur_scope->syms;
    while (sym) {
        struct sym *tmp = sym->next;
        free(sym);
        sym = tmp;
    }

    // Free scope
    free(cur_scope);
}

void sema_declare(struct cc3 *self, int sc, struct ty *ty, const char *name)
{
    struct sym *sym;

    // Find previous declaration in the current scope
    for (sym = self->cur_scope->syms; sym; sym = sym->next)
        if (strcmp(sym->name, name) == 0)
            break;

    if (sym) {
        // FIXME: allow re-declarations in some cases
        err("Re-declaration of symbol %s", name);
    } else {
        // Otherwise add new symbol to current scope
        sym = calloc(1, sizeof *sym);
        sym->next = self->cur_scope->syms;
        self->cur_scope->syms = sym;
    }

    sym->sc = sc;
    sym->ty = ty;
    sym->name = name;

    printf("Declare %s as ", name);
    print_ty(ty);
}

struct sym *sema_lookup(struct cc3 *self, const char *name)
{
    for (struct scope *scope = self->cur_scope; scope; scope = scope->parent)
        for (struct sym *sym = scope->syms; sym; sym = sym->next)
            if (strcmp(sym->name, name) == 0)
                return sym;
    return NULL;
}

struct ty *sema_findtypedef(struct cc3 *self, const char *name)
{
    struct sym *sym = sema_lookup(self, name);
    if (sym && sym->sc == TK_TYPEDEF) {
        return sym->ty;
    }
    return NULL;
}
