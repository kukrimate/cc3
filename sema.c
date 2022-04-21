#include "cc3.h"

memb_t *make_memb(ty_t *ty, char *name)
{
    memb_t *memb = calloc(1, sizeof *memb);
    if (!memb) abort();
    memb->ty = ty;
    memb->name = name;
    return memb;
}

memb_t *pack_struct(memb_t *members, int *out_align, int *out_size)
{
    for (memb_t *memb = members; memb; memb = memb->next) {
        int memb_align = ty_align(memb->ty);
        // The highest member alignment becomes the struct's alignment
        if (memb_align > *out_align)
            *out_align = memb_align;
        // Then we align the current size to the member's alignment
        // and that becomes the member's offset
        *out_size = align(*out_size, memb_align);
        memb->offset = *out_size;
        // Finally we increase the size by the member's size
        *out_size += ty_size(memb->ty);
    }

    // And then the whole struct's size must be aligned to be a multiple
    // of it's own alignment
    *out_size = align(*out_size, *out_align);

    return members;
}

memb_t *pack_union(memb_t *members, int *out_align, int *out_size)
{
    for (memb_t *memb = members; memb; memb = memb->next) {
        int memb_align = ty_align(memb->ty);
        int memb_size = ty_size(memb->ty);

        // The highest member alignment becomes the union's alignment
        if (memb_align > *out_align)
            *out_align = memb_align;

        // The highest member size becomes the union's size
        if (memb_size > *out_size)
            *out_size = memb_size;
    }

    return members;
}

ty_t *make_ty(int kind)
{
    ty_t *ty = calloc(1, sizeof *ty);
    if (!ty) abort();
    ty->kind = kind;
    return ty;
}

ty_t *make_ty_tag(tag_t *tag)
{
    ty_t *ty = make_ty(TY_TAG);
    ty->tag = tag;
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

    case TY_TAG:
        return make_ty_tag(ty->tag);

    case TY_POINTER:
        return make_pointer(clone_ty(ty->pointer.base_ty));

    case TY_ARRAY:
        return make_array(clone_ty(ty->array.elem_ty), ty->array.cnt);

    case TY_FUNCTION:
    {
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
    case TY_TAG:
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

static void print_ty_r(ty_t *ty);

static void print_members(memb_t *members)
{
    printf(" { ");
    for (memb_t *memb = members; memb; memb = memb->next) {
        print_ty_r(memb->ty);
        if (memb->name)
            printf(" %s", memb->name);
        printf("; ");
    }
    printf("}");
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

    case TY_TAG:
        switch (ty->tag->kind) {
        case TAG_STRUCT:
            if (ty->tag->name)
                printf("struct %s", ty->tag->name);
            else
                printf("struct");
            if (ty->tag->defined)
                print_members(ty->tag->members);
            break;
        case TAG_UNION:
            if (ty->tag->name)
                printf("union %s", ty->tag->name);
            else
                printf("union");
            if (ty->tag->defined)
                print_members(ty->tag->members);
            break;
        case TAG_ENUM:
            if (ty->tag->name)
                printf("enum %s", ty->tag->name);
            else
                printf("enum");
            break;
        }
        break;

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

    case TY_TAG:
        assert(ty->tag->defined);
        switch (ty->tag->kind) {
        case TAG_STRUCT:
        case TAG_UNION:
            return ty->tag->align;
        case TAG_ENUM:
            return 4;
        default:
            ASSERT_NOT_REACHED();
        }

    case TY_POINTER:
        return 8;

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

    case TY_TAG:
        assert(ty->tag->defined);
        switch (ty->tag->kind) {
        case TAG_STRUCT:
        case TAG_UNION:
            return ty->tag->size;
        case TAG_ENUM:
            return 4;
        default:
            ASSERT_NOT_REACHED();
        }

    case TY_POINTER:
        return 8;

    case TY_ARRAY:
        assert(ty->array.cnt != -1);
        return ty_size(ty->array.elem_ty) * ty->array.cnt;

    default:
        ASSERT_NOT_REACHED();
    }
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

static void free_members(memb_t *members)
{
    for (memb_t *memb = members; memb;) {
        memb_t *tmp = memb->next;
        free_ty(memb->ty);
        if (memb->name)
            free(memb->name);
        free(memb);
        memb = tmp;
    }
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

    // Free tags
    for (tag_t *tag = scope->tags; tag; ) {
        tag_t *tmp = tag->next;
        free(tag->name);
        if (tag->members)
            free_members(tag->members);
        free(tag);
        tag = tmp;
    }

    // Free scope
    free(scope);
}

sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, char *name)
{
    sym_t *sym;

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

    // Figure out symbol type from storage class
    switch (sc) {
    case TK_TYPEDEF:
        sym->kind = SYM_TYPEDEF;
        break;
    case TK_EXTERN:
        sym->kind = SYM_EXTERN;
        break;
    case TK_STATIC:
        sym->kind = SYM_STATIC;
        break;
    case TK_AUTO:
    case TK_REGISTER:
        sym->kind = SYM_LOCAL;
        break;
    case -1:
        if (self->scope->parent)
            sym->kind = SYM_LOCAL;
        else
            sym->kind = SYM_EXTERN;
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    // Variables cannot be declared as void
    if (sym->kind != SYM_TYPEDEF && ty->kind == TY_VOID)
        err("Tried to declare %s with type void", name);

    sym->ty = ty;
    sym->name = name;

    // Debug
    printf("Declare %s as ", name);
    print_ty(ty);

    if (self->scope->parent) {
        // First align the stack
        self->offset = align(self->offset, ty_align(ty));
        // Then we can allocate space for the variable
        sym->offset = self->offset;
        self->offset += ty_size(ty);
    } else {
        sym->offset = -1;
    }

    return sym;
}

sym_t *sema_lookup(sema_t *self, const char *name)
{
    for (scope_t *scope = self->scope; scope; scope = scope->parent)
        for (sym_t *sym = scope->syms; sym; sym = sym->next)
            if (!strcmp(sym->name, name))
                return sym;
    return NULL;
}

ty_t *sema_findtypedef(sema_t *self, const char *name)
{
    struct sym *sym = sema_lookup(self, name);
    if (sym && sym->kind == SYM_TYPEDEF) {
        return clone_ty(sym->ty);
    }
    return NULL;
}

static tag_t *create_tag(sema_t *self, int kind, const char *name)
{
    tag_t *tag = calloc(1, sizeof *tag);
    if (!tag) abort();
    tag->next = self->scope->tags;
    self->scope->tags = tag;
    tag->kind = kind;
    tag->name = name ? strdup(name) : NULL;
    return tag;
}

tag_t *sema_forward_declare_tag(sema_t *self, int kind, const char *name)
{
    // Look for tags in any scope
    tag_t *tag;
    for (scope_t *scope = self->scope; scope; scope = scope->parent)
        for (tag = scope->tags; tag; tag = tag->next)
            if (tag->name && !strcmp(tag->name, name))
                goto found;
found:

    if (tag) {
        // If there was forward declaration or definition in any scope, use that
        if (tag->kind != kind)
            err("Tag referred to with the wrong keyword");
    } else {
        // Create the in the tag current scope
        return create_tag(self, kind, name);
    }

    return tag;
}

tag_t *sema_define_tag(sema_t *self, int kind, const char *name)
{
    // Look for tags just in the current scope
    tag_t *tag;
    for (tag = self->scope->tags; tag; tag = tag->next)
        if (tag->name && !strcmp(tag->name, name))
            break;

    if (tag) {
        // If there was a forward declaration in the current scope, use that
        if (tag->kind != kind)
            err("Tag referred to with the wrong keyword");
        if (tag->defined)
            err("Re-definition of tag %s", name);
    } else {
        // Otherwise, create the tag in the current scope
        tag = create_tag(self, kind, name);
    }

    // In either case the tag will now become a defined
    tag->defined = true;
    return tag;
}

static expr_t *make_expr(int kind)
{
    expr_t *expr = calloc(1, sizeof *expr);
    if (!expr) abort();
    expr->kind = kind;
    return expr;
}

expr_t *make_sym(sema_t *self, const char *name)
{
    sym_t *sym = sema_lookup(self, name);

    // Find symbol
    if (!sym)
        err("Undeclared identifier %s", name);

    // Create expression based on symbol type
    expr_t *expr;
    if (sym->offset == -1) {
        expr = make_expr(EXPR_GLOBAL);
        expr->str = strdup(name);
    } else {
        expr = make_expr(EXPR_LOCAL);
        expr->offset = sym->offset;
    }
    expr->ty = clone_ty(sym->ty);
    return expr;
}

expr_t *make_const(ty_t *ty, val_t val)
{
    expr_t *expr = make_expr(EXPR_CONST);
    expr->ty = ty;
    expr->val = val;
    return expr;
}

expr_t *make_str_lit(const char *str)
{
    expr_t *expr = make_expr(EXPR_STR_LIT);
    expr->ty = make_array(make_ty(TY_CHAR), strlen(str) + 1);
    expr->str = strdup(str);
    return expr;
}

// Is ty an integer type?
static bool is_integer_ty(ty_t *ty)
{
    switch (ty->kind) {
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
    case TY_BOOL:
        return true;
    case TY_TAG:
        return ty->tag->kind == TAG_ENUM && ty->tag->defined;
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
    case TY_POINTER:
    case TY_ARRAY:
    case TY_FUNCTION:
        return false;
    default:
        ASSERT_NOT_REACHED();
    }
}

// Is ty an arithmetic type?
static bool is_arith_ty(ty_t *ty)
{
    switch (ty->kind) {
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
    case TY_BOOL:
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
        return true;
    case TY_TAG:
        return ty->tag->kind == TAG_ENUM && ty->tag->defined;
    case TY_POINTER:
    case TY_ARRAY:
    case TY_FUNCTION:
        return false;
    default:
        ASSERT_NOT_REACHED();
    }
}

// Is ty a scalar type?
static bool is_scalar_ty(ty_t *ty)
{
    switch (ty->kind) {
    // By definition scalar
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
    case TY_BOOL:
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
    case TY_POINTER:
    // Degrade to pointers in expressions
    case TY_ARRAY:
    case TY_FUNCTION:
        return true;
    // Might be non-scalar
    case TY_TAG:
        return ty->tag->kind == TAG_ENUM && ty->tag->defined;
    default:
        ASSERT_NOT_REACHED();
    }
}

// Does a value of type ty degrade to a pointer type and if so what is the base
// type of said pointer?
static ty_t *find_ptr_base(ty_t *ty)
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
    ty_t *ty;

    switch (kind) {
    case EXPR_REF:
        ty = make_pointer(arg1->ty);
        break;
    case EXPR_DREF:
        if (!(ty = find_ptr_base(arg1->ty)))
            err("Pointer type required");
        break;
    case EXPR_POS:
        if (!is_arith_ty((ty = arg1->ty)))
            err("Arihmetic type required");
        return arg1;
    case EXPR_NEG:
        if (!is_arith_ty((ty = arg1->ty)))
            err("Arihmetic type required");
        break;
    case EXPR_NOT:
        if (!is_integer_ty((ty = arg1->ty)))
            err("Integer type required");
        break;
    case EXPR_LNOT:
        if (!is_scalar_ty((ty = arg1->ty)))
        ty = make_ty(TY_INT);
        break;
    case EXPR_INIT:
        ty = NULL;  // Initializers don't have a type
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    expr_t *expr = make_expr(kind);
    expr->ty = ty;
    expr->arg1 = arg1;
    return expr;
}

expr_t *make_memb_expr(expr_t *arg1, const char *name)
{
    expr_t *expr = make_expr(EXPR_MEMB);
    expr->arg1 = arg1;

    if (arg1->ty->kind != TY_TAG)
        goto err;

    tag_t *tag = arg1->ty->tag;
    if (tag->kind != TAG_STRUCT && tag->kind != TAG_UNION)
        goto err;
    if (!tag->defined)
        goto err;

    for (memb_t *memb = tag->members; memb; memb = memb->next)
        if (!strcmp(memb->name, name)) {
            expr->ty = clone_ty(memb->ty);
            expr->offset = memb->offset;
            return expr;
        }

err:
    print_ty_r(arg1->ty);
    err(" has no member %s\n", name);
}

expr_t *make_binary(int kind, expr_t *arg1, expr_t *arg2)
{
    expr_t *expr = make_expr(kind);

    switch (kind) {
    case EXPR_CALL:
    {
        ty_t *func_ty = find_ptr_base(arg1->ty);
        if (!func_ty || func_ty->kind != TY_FUNCTION)
            err("Non-function object called");
        expr->ty = func_ty->function.ret_ty;
        break;
    }

    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
        expr->ty = arg1->ty;
        break;

    case EXPR_ADD:
    {
        ty_t *ty;

        if ((ty = find_ptr_base(arg1->ty))) {
            if (!is_integer_ty(arg2->ty))
                err("Pointer offset must be integer");
            arg2 = make_binary(EXPR_MUL, arg2,
                make_const(make_ty(TY_INT), ty_size(ty)));
            expr->ty = make_pointer(ty);
        } else if ((ty = find_ptr_base(arg2->ty))) {
            if (!is_integer_ty(arg1->ty))
                err("Pointer offset must be integer");
            arg1 = make_binary(EXPR_MUL, arg1,
                make_const(make_ty(TY_INT), ty_size(ty)));
            expr->ty = make_pointer(ty);
        } else {
            if (!is_arith_ty(arg1->ty) || !is_arith_ty(arg2->ty))
                err("Arihmetic type required");
            expr->ty = arg1->ty;
        }
        break;
    }

    case EXPR_SUB:
        expr->ty = arg1->ty;
        break;

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
