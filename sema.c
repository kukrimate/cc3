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

param_t *make_param(ty_t *ty, sym_t *sym)
{
    // Create parameter structure
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

ty_t *make_ty_tag(int kind, tag_t *tag)
{
    ty_t *ty = make_ty(kind);
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

ty_t *make_function(ty_t *ret_ty, scope_t *scope, param_t *params, bool var)
{
    ty_t *ty = make_ty(TY_FUNCTION);

    if (ret_ty->kind == TY_ARRAY || ret_ty->kind == TY_FUNCTION)
        err("Function cannot return %T", ret_ty);

    ty->function.ret_ty = ret_ty;
    ty->function.scope = scope;
    ty->function.params = params;
    ty->function.var = var;
    return ty;
}

static void print_members(tag_t *tag)
{
    assert(tag->defined);
    printf(" { ");
    tag->defined = false;   // Prevent runaway recursion
    for (memb_t *memb = tag->members; memb; memb = memb->next) {
        print_ty(memb->ty);
        if (memb->name)
            printf(" %s", memb->name);
        printf("; ");
    }
    tag->defined = true;
    printf("}");
}

void print_ty(ty_t *ty)
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

    case TY_STRUCT:
        if (ty->tag->name)
            printf("struct %s", ty->tag->name);
        else
            printf("struct");
        if (ty->tag->defined)
            print_members(ty->tag);
        break;

    case TY_UNION:
        if (ty->tag->name)
            printf("union %s", ty->tag->name);
        else
            printf("union");
        if (ty->tag->defined)
            print_members(ty->tag);
        break;

    case TY_ENUM:
        if (ty->tag->name)
            printf("enum %s", ty->tag->name);
        else
            printf("enum");
        break;

    case TY_POINTER:
        print_ty(ty->pointer.base_ty);
        printf("*");
        break;

    case TY_ARRAY:
        print_ty(ty->array.elem_ty);
        if (ty->array.cnt < 0)
            printf("[]");
        else
            printf("[%d]", ty->array.cnt);
        break;

    case TY_FUNCTION:
        print_ty(ty->function.ret_ty);

        printf("(");
        param_t *param = ty->function.params;
        while (param) {
            print_ty(param->ty);
            if (param->sym)
                printf(" %s", param->sym->name);
            if ((param = param->next) || ty->function.var)
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

    case TY_STRUCT:
    case TY_UNION:
        assert(ty->tag->defined);
        return ty->tag->align;

    case TY_ENUM:
        assert(ty->tag->defined);
        return 4;

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

    case TY_STRUCT:
    case TY_UNION:
        assert(ty->tag->defined);
        return ty->tag->size;

    case TY_ENUM:
        assert(ty->tag->defined);
        return 4;

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

    // FIXME: declare the real type of this
    sema_declare(self, TK_TYPEDEF,
        make_pointer(make_ty(TY_VOID)),
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
    // for (sym_t *sym = scope->syms; sym; ) {
    //     sym_t *tmp = sym->next;
    //     if (sym->kind != SYM_ENUM_CONST)
    //         free_ty(sym->ty);
    //     free(sym->name);
    //     free(sym);
    //     sym = tmp;
    // }

    // FIXME: Free tags, use refcounting
    // We cannot actually do this for now, because we will get use
    // after frees in the code generator
    // for (tag_t *tag = scope->tags; tag; ) {
    //     tag_t *tmp = tag->next;
    //     free(tag->name);
    //     if (tag->members)
    //         free_members(tag->members);
    //     free(tag);
    //     tag = tmp;
    // }

    // Free scope
    free(scope);
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
    debugln("Declare %s: %T", name, ty);

    // Look for previous declaration in the current scope
    sym_t *sym;
    for (sym = self->scope->syms; sym; sym = sym->next)
        if (!strcmp(sym->name, name))
            break;

    if (sym) {
        int new_kind = sym_kind(self, sc);

        switch (sym->kind) {
        case SYM_EXTERN:
        case SYM_STATIC:
            if (new_kind == SYM_EXTERN) { // OK
                break;
            }
            if (new_kind == SYM_STATIC) { // OK
                sym->kind = SYM_STATIC;
                break;
            }
            // FALLTHROUGH
        default:
            err("Invalid re-declaration of %s", name);
        }

        // FIXME: check re-declaration type
        // NOTE: This assignment is needed for function defs
        sym->ty = ty;

        return sym;
    }

    // Add new symbol to current scope
    sym = calloc(1, sizeof *sym);
    if (!sym) abort();
    sym->next = self->scope->syms;
    self->scope->syms = sym;

    sym->kind = sym_kind(self, sc);

    // Variables cannot be declared as void
    if (sym->kind != SYM_TYPEDEF && ty->kind == TY_VOID)
        err("Tried to declare %s with type void", name);

    sym->ty = ty;
    sym->name = name;

    // Allocate local variable if required
    if (sym->kind == SYM_LOCAL) {
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

sym_t *sema_declare_enum_const(sema_t *self, char *name, val_t val)
{
    // Look for previous declaration in the current scope
    for (sym_t *sym = self->scope->syms; sym; sym = sym->next)
        if (!strcmp(sym->name, name))
            err("Invalid re-declaration of %s", name);

    // If not found we are good to declare it
    sym_t *sym = calloc(1, sizeof *sym);
    if (!sym) abort();
    sym->next = self->scope->syms;
    self->scope->syms = sym;
    sym->kind = SYM_ENUM_CONST;
    sym->name = name;
    sym->val = val;
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
    if (sym && sym->kind == SYM_TYPEDEF)
        return sym->ty;
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
    tag_t *tag = NULL;

    if (name)
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
    tag_t *tag = NULL;

    if (name)
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
    // Special pre-declared identifier
    if (self->func_name && !strcmp(name, "__func__"))
        return make_str_lit(self->func_name);

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
        expr = make_expr(EXPR_SYM);
        expr->ty = sym->ty;
        expr->sym = sym;
        break;
    case SYM_ENUM_CONST:
        expr = make_const(make_ty(TY_INT), sym->val);
        break;
    }

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

static int find_memb(ty_t *ty, const char *name, ty_t **out)
{
    if (ty->kind != TY_STRUCT && ty->kind != TY_UNION)
        return -1;
    if (!ty->tag->defined)
        return -1;

    for (memb_t *memb = ty->tag->members; memb; memb = memb->next) {
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

expr_t *make_memb_expr(expr_t *arg1, const char *name)
{
    expr_t *expr = make_expr(EXPR_MEMB);
    expr->arg1 = arg1;

    expr->offset = find_memb(arg1->ty, name, &expr->ty);
    if (expr->offset == -1)
        err("Non-existent member %s of type %T", name, arg1->ty);
    return expr;
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
    #define FOLD_ARITH(op)                          \
        do {                                        \
            arg1->val = op arg1->val;               \
            return arg1;                            \
        } while (0)

    #define FOLD_LOGIC(op)                          \
        do {                                        \
            arg1->ty = make_ty(TY_INT);             \
            arg1->val = op arg1->val;               \
            return arg1;                            \
        } while (0)

    if (arg1->kind == EXPR_CONST)
        switch (kind) {
        case EXPR_NEG:  FOLD_ARITH(-);
        case EXPR_NOT:  FOLD_ARITH(~);
        case EXPR_LNOT: FOLD_LOGIC(!);
        }

    #undef FOLD_ARITH
    #undef FOLD_LOGIC

    switch (kind) {
    case EXPR_REF:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = make_pointer(arg1->ty);
        expr->arg1 = arg1;
        return expr;
    }
    case EXPR_DREF:
    {
        expr_t *expr = make_expr(kind);
        if (!(expr->ty = find_ptr_base(arg1->ty)))
            err("Pointer type required");
        expr->arg1 = arg1;
        return expr;
    }
    case EXPR_NEG:
    case EXPR_NOT:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = arg1->ty;
        expr->arg1 = arg1;
        return expr;
    }
    case EXPR_LNOT:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = make_ty(TY_INT);
        expr->arg1 = arg1;
        return expr;
    }
    case EXPR_INIT:
    {
        expr_t *expr = make_expr(kind);
        expr->arg1 = arg1;
        return expr;
    }
    default:
        ASSERT_NOT_REACHED();
    }
}

static expr_t *make_ptradd(ty_t *base_ty, expr_t *ptr, expr_t *idx)
{
    expr_t *expr = make_expr(EXPR_ADD);
    expr->ty = make_pointer(base_ty);
    expr->arg1 = ptr;
    expr->arg2 = make_binary(EXPR_MUL, idx,
        make_const(make_ty(TY_ULONG), ty_size(base_ty)));
    return expr;
}

static expr_t *make_ptrsub(ty_t *base_ty, expr_t *ptr1, expr_t *ptr2)
{
    expr_t *expr = make_expr(EXPR_SUB);
    expr->ty = make_ty(TY_LONG);
    expr->arg1 = ptr1;
    expr->arg1 = ptr2;
    return make_binary(EXPR_DIV, expr,
        make_const(make_ty(TY_INT), ty_size(base_ty)));
}

expr_t *make_binary(int kind, expr_t *arg1, expr_t *arg2)
{
    #define FOLD_ARITH(op)                      \
        do {                                    \
            arg1->val = arg1->val op arg2->val; \
            return arg1;                        \
        } while (0)

    #define FOLD_LOGIC(op)                      \
        do {                                    \
            arg1->ty = make_ty(TY_INT);         \
            arg1->val = arg1->val op arg2->val; \
            return arg1;                        \
        } while (0)

    if (arg1->kind == EXPR_CONST && arg2->kind == EXPR_CONST)
        switch (kind) {
        case EXPR_MUL:  FOLD_ARITH(*);
        case EXPR_DIV:  FOLD_ARITH(/);
        case EXPR_MOD:  FOLD_ARITH(%);
        case EXPR_ADD:  FOLD_ARITH(+);
        case EXPR_SUB:  FOLD_ARITH(-);
        case EXPR_LSH:  FOLD_ARITH(<<);
        case EXPR_RSH:  FOLD_ARITH(>>);
        case EXPR_AND:  FOLD_ARITH(&);
        case EXPR_XOR:  FOLD_ARITH(^);
        case EXPR_OR:   FOLD_ARITH(|);
        case EXPR_LT:   FOLD_LOGIC(<);
        case EXPR_GT:   FOLD_LOGIC(>);
        case EXPR_LE:   FOLD_LOGIC(<=);
        case EXPR_GE:   FOLD_LOGIC(>=);
        case EXPR_EQ:   FOLD_LOGIC(==);
        case EXPR_NE:   FOLD_LOGIC(!=);
        case EXPR_LAND: FOLD_LOGIC(&&);
        case EXPR_LOR:  FOLD_LOGIC(||);
        }

    #undef FOLD_ARITH
    #undef FOLD_LOGIC

    switch (kind) {
    case EXPR_CALL:
    {
        ty_t *func_ty = find_ptr_base(arg1->ty);
        if (!func_ty || func_ty->kind != TY_FUNCTION)
            err("Non-function object called");

        expr_t *expr = make_expr(kind);
        expr->ty = func_ty->function.ret_ty;
        expr->arg1 = arg1;
        expr->arg2 = arg2;
        return expr;
    }

    case EXPR_ADD:
    {
        ty_t *base_ty;
        if ((base_ty = find_ptr_base(arg1->ty)))
            return make_ptradd(base_ty, arg1, arg2);
        if ((base_ty = find_ptr_base(arg2->ty)))
            return make_ptradd(base_ty, arg2, arg1);
        goto make_arith;
    }

    case EXPR_SUB:
    {
        ty_t *base_ty1, *base_ty2;
        if ((base_ty1 = find_ptr_base(arg1->ty)) && (base_ty2 = find_ptr_base(arg2->ty)))
            return make_ptrsub(base_ty1, arg1, arg2);
        goto make_arith;
    }

    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
    case EXPR_LSH:
    case EXPR_RSH:
    case EXPR_AND:
    case EXPR_XOR:
    case EXPR_OR:
    make_arith:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = arg1->ty;
        expr->arg1 = arg1;
        expr->arg2 = arg2;
        return expr;
    }

    case EXPR_LT:
    case EXPR_GT:
    case EXPR_LE:
    case EXPR_GE:
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_LAND:
    case EXPR_LOR:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = make_ty(TY_INT);
        expr->arg1 = arg1;
        expr->arg2 = arg2;
        return expr;
    }

    case EXPR_AS:
    case EXPR_SEQ:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = arg2->ty;
        expr->arg1 = arg1;
        expr->arg2 = arg2;
        return expr;
    }

    default:
        ASSERT_NOT_REACHED();
    }
}

expr_t *make_trinary(int kind, expr_t *arg1, expr_t *arg2, expr_t *arg3)
{
    expr_t *expr = make_expr(kind);
    assert(kind == EXPR_COND);
    expr->ty = arg2->ty;

    if (arg1->kind == EXPR_CONST)
        return arg1->val ? arg2 : arg3;

    expr->arg1 = arg1;
    expr->arg2 = arg2;
    expr->arg3 = arg3;
    return expr;
}

expr_t *make_stmt_expr(stmt_t *body)
{
    expr_t *expr = make_expr(EXPR_STMT);

    // Get the type of the last eval (or void)
    stmt_t *last = body;
    while (last->next)
        last = last->next;
    if (last && last->kind == STMT_EVAL)
        expr->ty = last->arg1->ty;
    else
        expr->ty = make_ty(TY_VOID);

    expr->body = body;
    return expr;
}

stmt_t *make_stmt(int kind)
{
    stmt_t *stmt = calloc(1, sizeof *stmt);
    if (!stmt) abort();
    stmt->kind = kind;
    return stmt;
}
