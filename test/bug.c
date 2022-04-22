typedef int bool;
typedef struct ty ty_t;
typedef struct param param_t;
typedef unsigned long size_t;

void abort(void);
int printf(const char *, ...);
void *calloc (size_t, size_t);

struct param {
    param_t *next;
    ty_t *ty;
};

enum {
    TY_VOID,
    TY_CHAR,
    TY_SCHAR,
    TY_UCHAR,
    TY_SHORT,
    TY_USHORT,
    TY_INT,
    TY_UINT,
    TY_LONG,
    TY_ULONG,
    TY_LLONG,
    TY_ULLONG,
    TY_FLOAT,
    TY_DOUBLE,
    TY_LDOUBLE,
    TY_BOOL,
    TY_POINTER,
    TY_ARRAY,
    TY_FUNCTION,
};

struct ty {
    int kind;

    union {
        struct {
            ty_t *base_ty;
        } pointer;

        struct {
            ty_t *elem_ty;
            int cnt;
        } array;

        struct {
            ty_t *ret_ty;
            param_t *params;
            bool var;
        } function;
    };
};

param_t *make_param(ty_t *ty)
{
    param_t *param = calloc(1, sizeof *param);
    if (!param) abort();
    param->ty = ty;
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

ty_t *make_function(ty_t *ret_ty, param_t *params, bool var)
{
    ty_t *ty = make_ty(TY_FUNCTION);
    ty->function.ret_ty = ret_ty;
    ty->function.params = params;
    ty->function.var = var;
    return ty;
}

void print_ty(ty_t *ty);

void print_params(param_t *params, bool var)
{
    printf("(");
    param_t *param = params;
    while (param) {
        print_ty(param->ty);
        // BUG: This compound assignment was generated twice because
        // the boolean code generator had a typo
        if ((param = param->next) || var)
            printf(", ");
    }
    if (var)
        printf("...");
    printf(")");
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
        print_params(ty->function.params, ty->function.var);
        break;
    }
}

int main(void)
{
    param_t *params = make_param(make_ty(TY_INT));
    ty_t *ty = make_function(make_pointer(make_ty(TY_INT)), params, 0);

    print_ty(ty);
    printf("\n");
}
