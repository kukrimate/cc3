// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

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
        printf("%d\n", ty->kind);
        ASSERT_NOT_REACHED();
    }
}

// God forgive my RPN sins

void print_expr(expr_t *expr)
{
    switch (expr->kind) {
    case EXPR_SYM:
        printf("%s", expr->as_sym->name);
        break;

    case EXPR_CONST:
        printf("%lld", expr->as_const.value);
        break;

    case EXPR_STR:
        // FIXME: escape unprintable and control characters
        printf("\"%s\"", expr->as_str.data);
        break;

    case EXPR_MEMB:
        printf("MEMB ");
        print_expr(expr->as_memb.aggr);
        printf(" %s", expr->as_memb.name);
        break;

    case EXPR_CALL:
        putchar('(');
        print_expr(expr->as_call.func);
        VEC_FOREACH(&expr->as_call.args, arg) {
            putchar(' ');
            print_expr(*arg);
        }
        putchar(')');
        break;


    case EXPR_CAST:
        printf("CAST ");
        print_ty(expr->ty);
        putchar(' ');
        print_expr(expr->as_unary.arg);
        break;

    case EXPR_REF:
    case EXPR_DREF:
    case EXPR_NEG:
    case EXPR_NOT:
    case EXPR_LNOT:
        switch (expr->kind) {
        case EXPR_REF:
            printf("ADDR");
            break;
        case EXPR_DREF:
            printf("IND");
            break;
        case EXPR_NEG:
            printf("NEG");
            break;
        case EXPR_NOT:
            printf("NOT");
            break;
        case EXPR_LNOT:
            printf("LNOT");
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        putchar(' ');
        print_expr(expr->as_unary.arg);
        break;

    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
    case EXPR_ADD:
    case EXPR_SUB:
    case EXPR_LSH:
    case EXPR_RSH:
    case EXPR_LT:
    case EXPR_GT:
    case EXPR_LE:
    case EXPR_GE:
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_AND:
    case EXPR_XOR:
    case EXPR_OR:
    case EXPR_LAND:
    case EXPR_LOR:
    case EXPR_AS:
    case EXPR_SEQ:
        switch (expr->kind) {
        case EXPR_MUL:      putchar('*');   break;
        case EXPR_DIV:      putchar('/');   break;
        case EXPR_MOD:      putchar('%');   break;
        case EXPR_ADD:      putchar('+');   break;
        case EXPR_SUB:      putchar('-');   break;
        case EXPR_LSH:      printf("<<");   break;
        case EXPR_RSH:      printf(">>");   break;
        case EXPR_LT:       putchar('<');   break;
        case EXPR_GT:       putchar('>');   break;
        case EXPR_LE:       printf("<=");   break;
        case EXPR_GE:       printf(">=");   break;
        case EXPR_EQ:       printf("==");   break;
        case EXPR_NE:       printf("!=");   break;
        case EXPR_AND:      putchar('&');   break;
        case EXPR_XOR:      putchar('^');   break;
        case EXPR_OR:       putchar('|');   break;
        case EXPR_LAND:     printf("&&");   break;
        case EXPR_LOR:      printf("||");   break;
        case EXPR_AS:       putchar('=');   break;
        case EXPR_SEQ:      putchar(',');   break;
        default:            ASSERT_NOT_REACHED();
        }
        putchar(' ');
        print_expr(expr->as_binary.lhs);
        putchar(' ');
        print_expr(expr->as_binary.rhs);
        break;

    case EXPR_COND:
        printf("? ");
        print_expr(expr->as_trinary.arg1);
        putchar(' ');
        print_expr(expr->as_trinary.arg2);
        putchar(' ');
        print_expr(expr->as_trinary.arg3);
        break;

    case EXPR_STMT:
        printf("({");
        print_stmts(expr->as_stmt, 0);
        printf("})");
        break;

    case EXPR_VA_START:
        printf("(va_start ");
        print_expr(expr->as_unary.arg);
        putchar(')');
        break;
    case EXPR_VA_END:
        printf("(va_end ");
        print_expr(expr->as_unary.arg);
        putchar(')');
        break;
    case EXPR_VA_ARG:
        printf("(va_arg ");
        print_expr(expr->as_unary.arg);
        putchar(' ');
        print_ty(expr->ty);
        putchar(')');
        break;

    default:
        ASSERT_NOT_REACHED();
    }
}


static void iprint(int indent, char *fmt, ...)
{
    while (indent--)
        printf("  ");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

void print_stmts(stmt_t *stmt, int indent)
{
    for (; stmt; stmt = stmt->next) {
        switch (stmt->kind) {
        case STMT_LABEL:
            iprint(indent, "%s:\n", stmt->as_label.label);
            break;
        case STMT_CASE:
            if (stmt->as_case.begin != stmt->as_case.end)
                iprint(indent, "case %d ... %d:\n",
                    stmt->as_case.begin,
                    stmt->as_case.end);
            else
                iprint(indent, "case %d:\n", stmt->as_case.begin);
            break;
        case STMT_DEFAULT:
            iprint(indent, "default:\n");
            break;
        case STMT_IF:
            iprint(indent, "if (");
            print_expr(stmt->as_if.cond);
            printf(") {\n");
            print_stmts(stmt->as_if.then_body, indent + 1);
            if (stmt->as_if.else_body) {
                iprint(indent, "} else {\n");
                print_stmts(stmt->as_if.else_body, indent + 1);
            }
            iprint(indent, "}\n");
            break;
        case STMT_SWITCH:
            iprint(indent, "switch (");
            print_expr(stmt->as_switch.cond);
            printf(") {\n");
            print_stmts(stmt->as_switch.body, indent + 1);
            iprint(indent, "}\n");
            break;
        case STMT_WHILE:
            iprint(indent, "while (");
            print_expr(stmt->as_while.cond);
            printf(") {\n");
            print_stmts(stmt->as_while.body, indent + 1);
            iprint(indent, "}\n");
            break;
        case STMT_DO:
            iprint(indent, "do {\n");
            print_stmts(stmt->as_do.body, indent + 1);
            iprint(indent, "} while (");
            print_expr(stmt->as_do.cond);
            printf(");\n");
            break;
        case STMT_FOR:
            iprint(indent, "for (");
            if (stmt->as_for.init)
                print_expr(stmt->as_for.init);
            printf(";");
            if (stmt->as_for.cond) {
                putchar(' ');
                print_expr(stmt->as_for.cond);
            }
            printf(";");
            if (stmt->as_for.incr) {
                putchar(' ');
                print_expr(stmt->as_for.incr);
            }
            printf(") {\n");
            print_stmts(stmt->as_for.body, indent + 1);
            iprint(indent, "}\n");
            break;
        case STMT_GOTO:
            iprint(indent, "goto %s;\n", stmt->as_goto.label);
            break;
        case STMT_CONTINUE:
            iprint(indent, "continue;\n");
            break;
        case STMT_BREAK:
            iprint(indent, "break;\n");
            break;
        case STMT_RETURN:
            iprint(indent, "return");
            if (stmt->as_return.value) {
                putchar(' ');
                print_expr(stmt->as_return.value);
            }
            printf(";\n");
            break;
        case STMT_EVAL:
            iprint(indent, "");
            print_expr(stmt->as_eval.value);
            printf(";\n");
            break;
        case STMT_DECL:
            // FIXME: print initializer too
            iprint(indent, "declare %s as ", stmt->as_decl.sym->name);
            print_ty(stmt->as_decl.sym->ty);
            printf(";\n");
            break;

        default:
            ASSERT_NOT_REACHED();
        }
    }
}
