#include "cc3.h"

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
        int memb_align = ty_align(memb->ty);
        // The highest member alignment becomes the struct's alignment
        if (memb_align > ty->as_aggregate.align)
            ty->as_aggregate.align = memb_align;
        // Then we align the current size to the member's alignment
        // and that becomes the member's offset
        ty->as_aggregate.size = align(ty->as_aggregate.size, memb_align);
        memb->offset = ty->as_aggregate.size;
        // Finally we increase the size by the member's size
        ty->as_aggregate.size += ty_size(memb->ty);
    }
    // And then the whole struct's size must be aligned to be a multiple
    // of it's own alignment
    ty->as_aggregate.size = align(ty->as_aggregate.size, ty->as_aggregate.align);
}

void define_union(ty_t *ty, memb_t *members)
{
    if (ty->as_aggregate.members)
        err("Re-definition of union");

    ty->as_aggregate.members = members;

    for (memb_t *memb = members; memb; memb = memb->next) {
        int memb_align = ty_align(memb->ty);
        int memb_size = ty_size(memb->ty);
        // The highest member alignment becomes the union's alignment
        if (memb_align > ty->as_aggregate.align)
            ty->as_aggregate.align = memb_align;
        // The highest member size becomes the union's size
        if (memb_size > ty->as_aggregate.size)
            ty->as_aggregate.size = memb_size;
    }
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

static void print_members(memb_t *members)
{
    printf(" { ");
    for (memb_t *memb = members; memb; memb = memb->next) {
        print_ty(memb->ty);
        if (memb->name)
            printf(" %s", memb->name);
        printf("; ");
    }
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
        if (ty->as_aggregate.name) {
            printf("struct %s", ty->as_aggregate.name);
        } else {
            printf("struct");
            assert(ty->as_aggregate.members);
            print_members(ty->as_aggregate.members);
        }
        break;

    case TY_UNION:
        if (ty->as_aggregate.name) {
            printf("union %s", ty->as_aggregate.name);
        } else {
            printf("union");
            assert(ty->as_aggregate.members);
            print_members(ty->as_aggregate.members);
        }
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
        assert(ty->as_aggregate.members);
        return ty->as_aggregate.align;

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
        assert(ty->as_aggregate.members);
        return ty->as_aggregate.size;

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
    *tail = make_memb(make_ty(TY_UINT), "gp_offset");
    tail = &(*tail)->next;
    *tail = make_memb(make_ty(TY_UINT), "fp_offset");
    tail = &(*tail)->next;
    *tail = make_memb(make_pointer(make_ty(TY_VOID)), "overflow_arg_area");
    tail = &(*tail)->next;
    *tail = make_memb(make_pointer(make_ty(TY_VOID)), "reg_save_area");
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
}

void sema_exit(sema_t *self)
{
    scope_t *scope = self->scope;
    self->scope = scope->parent;
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

    // Create unique assembler name for block scope static
    if (sym->kind == SYM_STATIC && self->scope->parent) {
        string_t s;
        string_init(&s);
        string_printf(&s, "%s$%d", sym->name, self->block_static_cnt++);
        sym->asm_name = s.data;
    } else {
        sym->asm_name = sym->name;
    }

    return sym;
}

void sema_declare_end(sema_t *self, sym_t *sym)
{
    if (sym->kind == SYM_LOCAL) {
        // First align the stack
        self->offset = align(self->offset, ty_align(sym->ty));
        // Then we can allocate space for the variable
        sym->offset = self->offset;
        self->offset += ty_size(sym->ty);
    }
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

static void create_tag(sema_t *self, ty_t *ty)
{
    tag_t *tag = calloc(1, sizeof *tag);
    if (!tag) abort();
    tag->next = self->scope->tags;
    self->scope->tags = tag;
    tag->ty = ty;
}

ty_t *sema_forward_declare_tag(sema_t *self, int kind, const char *name)
{
    // Look for tags in any scope
    tag_t *tag = NULL;
    for (scope_t *scope = self->scope; scope; scope = scope->parent)
        for (tag = scope->tags; tag; tag = tag->next)
            if (!strcmp(tag->ty->as_aggregate.name, name))
                goto found;
found:

    if (tag) {
        // Make sure the right keyword is used to refer to the tag
        if (tag->ty->kind != kind)
            err("Tag referred to with the wrong keyword");
        // Return the type associated with the tag we found
        return tag->ty;
    } else {
        // If no tag was found, we create one in the current scope
        ty_t *ty = make_ty(kind);
        ty->as_aggregate.name = strdup(name);
        create_tag(self, ty);
        return ty;
    }
}

ty_t *sema_define_tag(sema_t *self, int kind, const char *name)
{
    // Look for tags just in the current scope
    tag_t *tag = NULL;
    for (tag = self->scope->tags; tag; tag = tag->next)
        if (!strcmp(tag->ty->as_aggregate.name, name))
            break;

    if (tag) {
        // Make sure the right keyword is used to refer to the tag
        if (tag->ty->kind != kind)
            err("Tag referred to with the wrong keyword");
        // Return the type associated with the tag we found
        return tag->ty;
    } else {
        // If no tag was found, we create one in the current scope
        ty_t *ty = make_ty(kind);
        ty->as_aggregate.name = strdup(name);
        create_tag(self, ty);
        return ty;
    }
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
        expr->as_sym = sym;
        break;
    case SYM_ENUM_CONST:
        expr = make_const(make_ty(TY_INT), sym->val);
        break;
    }

    return expr;
}

expr_t *make_const(ty_t *ty, val_t value)
{
    expr_t *expr = make_expr(EXPR_CONST);
    expr->ty = ty;
    expr->as_const.value = value;
    return expr;
}

expr_t *make_str_lit(const char *data)
{
    expr_t *expr = make_expr(EXPR_STR_LIT);
    expr->ty = make_array(make_ty(TY_CHAR), strlen(data) + 1);
    expr->as_str_lit.data = strdup(data);
    return expr;
}

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
    expr_t *expr = make_expr(EXPR_MEMB);
    expr->as_memb.aggr = aggr;
    expr->as_memb.offset = find_memb(aggr->ty, name, &expr->ty);
    if (expr->as_memb.offset == -1)
        err("Non-existent member %s of type %T", name, aggr->ty);
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

expr_t *make_unary(int kind, expr_t *arg)
{
    #define FOLD_ARITH(op)                                  \
        do {                                                \
            arg->as_const.value = op arg->as_const.value;   \
            return arg;                                     \
        } while (0)

    #define FOLD_LOGIC(op)                                  \
        do {                                                \
            arg->ty = make_ty(TY_INT);                      \
            arg->as_const.value = op arg->as_const.value;   \
            return arg;                                     \
        } while (0)

    if (arg->kind == EXPR_CONST)
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
        expr->ty = make_pointer(arg->ty);
        expr->as_unary.arg = arg;
        return expr;
    }
    case EXPR_DREF:
    {
        expr_t *expr = make_expr(kind);
        if (!(expr->ty = find_ptr_base(arg->ty)))
            err("Pointer type required");
        expr->as_unary.arg = arg;
        return expr;
    }
    case EXPR_NEG:
    case EXPR_NOT:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = arg->ty;
        expr->as_unary.arg = arg;
        return expr;
    }
    case EXPR_LNOT:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = make_ty(TY_INT);
        expr->as_unary.arg = arg;
        return expr;
    }
    case EXPR_VA_START:
    case EXPR_VA_END:
    case EXPR_VA_ARG:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = make_ty(TY_VOID);
        expr->as_unary.arg = arg;
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
    expr->as_binary.lhs = ptr;
    expr->as_binary.rhs = make_binary(EXPR_MUL, idx,
        make_const(make_ty(TY_ULONG), ty_size(base_ty)));
    return expr;
}

static expr_t *make_ptrsub(ty_t *base_ty, expr_t *ptr1, expr_t *ptr2)
{
    expr_t *expr = make_expr(EXPR_SUB);
    expr->ty = make_ty(TY_LONG);
    expr->as_binary.lhs = ptr1;
    expr->as_binary.rhs = ptr2;
    return make_binary(EXPR_DIV, expr,
        make_const(make_ty(TY_INT), ty_size(base_ty)));
}

expr_t *make_binary(int kind, expr_t *lhs, expr_t *rhs)
{
    #define FOLD_ARITH(op)                                  \
        do {                                                \
            lhs->as_const.value =                           \
                lhs->as_const.value op rhs->as_const.value; \
            return lhs;                                     \
        } while (0)

    #define FOLD_LOGIC(op)                                  \
        do {                                                \
            lhs->ty = make_ty(TY_INT);                      \
            lhs->as_const.value =                           \
                lhs->as_const.value op rhs->as_const.value; \
            return lhs;                                     \
        } while (0)

    if (lhs->kind == EXPR_CONST && rhs->kind == EXPR_CONST)
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
        ty_t *func_ty = find_ptr_base(lhs->ty);
        if (!func_ty || func_ty->kind != TY_FUNCTION)
            err("Non-function object called");

        expr_t *expr = make_expr(kind);
        expr->ty = func_ty->function.ret_ty;
        expr->as_binary.lhs = lhs;
        expr->as_binary.rhs = rhs;
        return expr;
    }

    case EXPR_ADD:
    {
        ty_t *base_ty;
        if ((base_ty = find_ptr_base(lhs->ty)))
            return make_ptradd(base_ty, lhs, rhs);
        if ((base_ty = find_ptr_base(rhs->ty)))
            return make_ptradd(base_ty, lhs, rhs);
        goto make_arith;
    }

    case EXPR_SUB:
    {
        ty_t *base_ty1, *base_ty2;
        if ((base_ty1 = find_ptr_base(lhs->ty)) && (base_ty2 = find_ptr_base(rhs->ty)))
            return make_ptrsub(base_ty1, lhs, rhs);
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
        expr->ty = lhs->ty;
        expr->as_binary.lhs = lhs;
        expr->as_binary.rhs = rhs;
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
        expr->as_binary.lhs = lhs;
        expr->as_binary.rhs = rhs;
        return expr;
    }

    case EXPR_AS:
    case EXPR_SEQ:
    {
        expr_t *expr = make_expr(kind);
        expr->ty = rhs->ty;
        expr->as_binary.lhs = lhs;
        expr->as_binary.rhs = rhs;
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
        return arg1->as_const.value ? arg2 : arg3;

    expr->as_trinary.arg1 = arg1;
    expr->as_trinary.arg2 = arg2;
    expr->as_trinary.arg3 = arg3;
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
        expr->ty = last->as_eval.value->ty;
    else
        expr->ty = make_ty(TY_VOID);

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
        return make_init_expr(make_const(make_ty(TY_INT), 0));

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
    if (init->as_expr->kind != EXPR_STR_LIT)
        goto bail;

    // The string literal might complete the array's type
    if (ty->array.cnt == -1)
        ty->array.cnt = strlen(init->as_expr->as_str_lit.data) + 1;

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
        for (ty->array.cnt = 0; *iter; ++ty->array.cnt) {
            *tail = bind_init_r(ty->array.elem_ty, iter, false);
            tail = &(*tail)->next;
        }
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
