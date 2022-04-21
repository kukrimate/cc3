#include "cc3.h"

void gen_init(gen_t *self)
{
    self->str_lit_cnt = 0;
    string_init(&self->code);
    string_init(&self->data);
}

void gen_free(gen_t *self)
{
    string_free(&self->code);
    string_free(&self->data);
}

static void emit_data(gen_t *self, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    string_vprintf(&self->data, fmt, ap);
    va_end(ap);
}

static void emit_code(gen_t *self, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    string_vprintf(&self->code, fmt, ap);
    va_end(ap);
}

static void emit_str_lit(gen_t *self, const char *s)
{
    // Create global index for literal
    int idx = self->str_lit_cnt++;

    // Emit name
    emit_data(self, "str$%d db ", idx);
    // Emit chars
    while (*s)
        emit_data(self, "%02Xh, ", *s++);
    // Emit NUL
    emit_data(self, "0\n");

    // Emit reference in code
    emit_code(self, "mov rax, str$%d\n", idx);
}

static int next_label(gen_t *self)
{
    return self->label_cnt++;
}

static void emit_target(gen_t *self, int label)
{
    emit_code(self, ".L%d:\n", label);
}

static void emit_jump(gen_t *self, const char *op, int label)
{
    emit_code(self, "%s .L%d\n", op, label);
}

static void gen_addr(gen_t *self, expr_t *expr);
static void gen_value(gen_t *self, expr_t *expr);
static void gen_bool(gen_t *self, expr_t *expr, bool val, int label);

void gen_addr(gen_t *self, expr_t *expr)
{
    switch (expr->kind) {
        case EXPR_GLOBAL:
            emit_code(self, "mov rax, %s\n", expr->str);
            break;
        case EXPR_LOCAL:
            emit_code(self, "lea rax, [rbp - %d]\n", self->offset - expr->offset);
            break;
        case EXPR_STR_LIT:
            emit_str_lit(self, expr->str);
            break;
        case EXPR_MEMB:
            gen_addr(self, expr->arg1);
            if (expr->offset)
                emit_code(self, "add rax, %d\n", expr->offset);
            break;
        case EXPR_DREF:
            gen_value(self, expr->arg1);
            break;
        default:
            err("Expected lvalue expression");
    }
}

static inline const char *bop(int kind)
{
    // FIXME: signedness
    switch (kind) {
    case EXPR_MUL:  return "imul rax, rcx";
    case EXPR_DIV:  return "xor edx, edx\ndiv rcx";
    case EXPR_MOD:  return "xor edx, edx\ndiv rcx\nmov rax, rdx";
    case EXPR_ADD:  return "add rax, rcx";
    case EXPR_SUB:  return "sub rax, rcx";
    case EXPR_LSH:  return "shl rax, cl";
    case EXPR_RSH:  return "shr rax, cl";
    case EXPR_AND:  return "and rax, rcx";
    case EXPR_XOR:  return "xor rax, rcx";
    case EXPR_OR:   return "or rax, rcx";
    default:        ASSERT_NOT_REACHED();
    }
}

void gen_value(gen_t *self, expr_t *expr)
{
    static const char *arg_regs[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

    switch (expr->kind) {
    case EXPR_GLOBAL:
    case EXPR_LOCAL:
    case EXPR_STR_LIT:
    case EXPR_MEMB:
    case EXPR_DREF:
        // Generate lvalue address
        gen_addr(self, expr);

        switch (expr->ty->kind) {
        // Extend smaller types to reguster
        case TY_CHAR:
        case TY_SCHAR:
            emit_code(self, "movsx rax, byte [rax]\n");
            break;
        case TY_BOOL:
        case TY_UCHAR:
            emit_code(self, "movzx rax, byte [rax]\n");
            break;
        case TY_SHORT:
            emit_code(self, "movsx rax, word [rax]\n");
            break;
        case TY_USHORT:
            emit_code(self, "movzx rax, word [rax]\n");
            break;
        case TY_INT:
            emit_code(self, "movsxd rax, dword [rax]\n");
            break;
        case TY_UINT:
            emit_code(self, "movsxd rax, dword [rax]\n");
            break;

        // Native qword loads
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit_code(self, "mov rax, qword [rax]\n");
            break;

        // FIXME: float loads
        case TY_FLOAT:
        case TY_DOUBLE:
        case TY_LDOUBLE:
            ASSERT_NOT_REACHED();

        // Other types are left as addresses
        default:
            break;
        }

        break;

    case EXPR_CONST:
        emit_code(self, "mov rax, %llu\n", expr->val);
        break;

    case EXPR_CALL:
    {
        // Generate arguments
        expr_t *arg = expr->arg2;
        const char **reg = arg_regs;
        while (arg) {
            gen_value(self, arg);
            emit_code(self, "mov %s, rax\n", *reg);
            arg = arg->next;
            ++reg;
        }

        // Generate call target
        gen_value(self, expr->arg1);
        emit_code(self, "mov rbx, rax\n");

        // Do the call (zero rax for varargs)
        emit_code(self, "xor eax, eax\ncall rbx\n");
        break;
    }

    case EXPR_REF:
        gen_addr(self, expr->arg1);
        break;

    case EXPR_NEG:
        emit_code(self, "neg rax\n");
        break;
    case EXPR_NOT:
        emit_code(self, "not rax\n");
        break;

    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
    case EXPR_ADD:
    case EXPR_SUB:
    case EXPR_LSH:
    case EXPR_RSH:
    case EXPR_AND:
    case EXPR_XOR:
    case EXPR_OR:
        gen_value(self, expr->arg1);
        emit_code(self, "push rax\n");
        gen_value(self, expr->arg2);
        emit_code(self,
            "mov rcx, rax\n"
            "pop rax\n"
            "%s\n",
            bop(expr->kind));
        break;

    case EXPR_LT:
    case EXPR_GT:
    case EXPR_LE:
    case EXPR_GE:
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_LNOT:
    case EXPR_LAND:
    case EXPR_LOR:
    {
        // Compile boolean expression to jumping code as normal
        int lfalse = next_label(self), lend = next_label(self);
        gen_bool(self, expr, false, lfalse);
        // Then add the landing zone that sets rax based on the target
        emit_code(self, "mov eax, 1\n");
        emit_jump(self, "jmp", lend);
        emit_target(self, lfalse);
        emit_code(self, "mov eax, 0\n");
        emit_target(self, lend);
        break;
    }

    case EXPR_AS:
    {
        gen_value(self, expr->arg2);
        emit_code(self, "push rax\n");
        gen_addr(self, expr->arg1);
        emit_code(self,
            "mov rbx, rax\n"
            "pop rax\n");
        switch (expr->ty->kind) {
        case TY_CHAR:
        case TY_SCHAR:
        case TY_UCHAR:
        case TY_BOOL:
            emit_code(self, "mov byte [rbx], al\n");
            break;
        case TY_SHORT:
        case TY_USHORT:
            emit_code(self, "mov word [rbx], ax\n");
            break;
        case TY_INT:
        case TY_UINT:
            emit_code(self, "mov dword [rbx], eax\n");
            break;
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit_code(self, "mov qword [rbx], rax\n");
            break;
        // FIXME: float loads
        case TY_FLOAT:
        case TY_DOUBLE:
        case TY_LDOUBLE:
            ASSERT_NOT_REACHED();
        // Other types are left as addresses
        default:
            break;
        }
        break;
    }

    case EXPR_COND:
    {
        // Compile arg1 as the condition
        int lfalse = next_label(self), lend = next_label(self);
        gen_bool(self, expr->arg1, false, lfalse);
        // Evaluate arg2 in the true case
        gen_value(self, expr->arg2);
        emit_jump(self, "jmp", lend);
        emit_target(self, lfalse);
        // Evaluate arg3 in the true case
        gen_value(self, expr->arg3);
        emit_target(self, lend);
        break;
    }

    case EXPR_SEQ:
        gen_value(self, expr->arg1);
        gen_value(self, expr->arg2);
        break;

    default:
        ASSERT_NOT_REACHED();
    }
}

static inline const char *jcc(int kind, bool val)
{
    // FIXME: signedness
    switch (kind) {
    case EXPR_LT:   return val ? "jl" : "jge";
    case EXPR_GT:   return val ? "jg" : "jle";
    case EXPR_LE:   return val ? "jle" : "jg";
    case EXPR_GE:   return val ? "jge" : "jl";
    case EXPR_EQ:   return val ? "je" : "jne";
    case EXPR_NE:   return val ? "jne" : "je";
    default:        ASSERT_NOT_REACHED();
    }
}

void gen_bool(gen_t *self, expr_t *expr, bool val, int label)
{
    switch (expr->kind) {
    // Boolean expression
    case EXPR_LT:
    case EXPR_GT:
    case EXPR_LE:
    case EXPR_GE:
    case EXPR_EQ:
    case EXPR_NE:
        // Generate comparison
        gen_value(self, expr->arg1);
        emit_code(self, "push rax\n");
        gen_value(self, expr->arg2);
        emit_code(self,
            "mov rcx, rax\n"
            "pop rax\n"
            "cmp rax, rcx\n");
        // Generate jump
        emit_jump(self, jcc(expr->kind, val), label);
        break;
    case EXPR_LNOT:
        gen_bool(self, expr->arg1, !val, label);
        break;
    case EXPR_LAND:
        if (val) {
            int lfall = next_label(self);
            gen_bool(self, expr->arg1, false, lfall);
            gen_bool(self, expr->arg1, true, label);
            emit_target(self, lfall);
        } else {
            gen_bool(self, expr->arg1, false, label);
            gen_bool(self, expr->arg2, false, label);
        }
        break;
    case EXPR_LOR:
        if (val) {
            gen_bool(self, expr->arg1, true, label);
            gen_bool(self, expr->arg2, true, label);
        } else {
            int lfall = next_label(self);
            gen_bool(self, expr->arg1, true, lfall);
            gen_bool(self, expr->arg1, false, label);
            emit_target(self, lfall);
        }
        break;
    // Test for value
    default:
        gen_value(self, expr);
        emit_code(self, "test rax, rax\n");
        emit_jump(self, val ? "jnz" : "jz", label);
        break;
    }
}

static void gen_init_value(gen_t *self, expr_t *expr)
{
    // Either the value itself or zero
    if (expr)
        gen_value(self, expr);
    else
        emit_code(self, "xor rax, rax\n");
}

static void gen_initializer(gen_t *self, ty_t *ty, int offset, expr_t *expr)
{
    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_BOOL:
        gen_init_value(self, expr);
        emit_code(self, "mov byte [rbp - %d], al\n", self->offset - offset);
        break;
    case TY_SHORT:
    case TY_USHORT:
        gen_init_value(self, expr);
        emit_code(self, "mov word [rbp - %d], ax\n", self->offset - offset);
        break;
    case TY_INT:
    case TY_UINT:
        gen_init_value(self, expr);
        emit_code(self, "mov dword [rbp - %d], eax\n", self->offset - offset);
        break;
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        gen_init_value(self, expr);
        emit_code(self, "mov qword [rbp - %d], rax\n", self->offset - offset);
        break;
    case TY_TAG:
        switch (ty->tag->kind) {
        case TAG_STRUCT:
            if (expr) {
                if (expr->kind != EXPR_INIT)
                    err("Expected initializer list");
                expr = expr->arg1;
            }

            // Struct initializers can initialize all entries
            for (memb_t *memb = ty->tag->members; memb; memb = memb->next) {
                gen_initializer(self, memb->ty, offset + memb->offset, expr);
                if (expr)
                    expr = expr->next;
            }

            // Make sure the initializer list has no more elements
            if (expr)
                err("Trailing garbage in initializer list");

            break;

        case TAG_UNION:
            if (expr) {
                if (expr->kind != EXPR_INIT)
                    err("Expected initializer list");
                expr = expr->arg1;
            }

            // Union initializers can only initializer the first member
            memb_t *memb = ty->tag->members;
            gen_initializer(self, memb->ty, offset + memb->offset, expr);

            // Make sure the initializer list has no more elements
            if (expr->next)
                err("Trailing garbage in initializer list");

            break;
        case TAG_ENUM:
            // Same as int
            if (expr)
                gen_value(self, expr);
            else
                emit_code(self, "xor rax, rax\n");
            emit_code(self, "mov dword [rbp - %d], eax\n", self->offset - offset);
            break;
        }
        break;
    case TY_ARRAY:
        {
            if (expr) {
                if (expr->kind != EXPR_INIT)
                    err("Expected initializer list");
                expr = expr->arg1;
            }

            // Save array element type and its size
            ty_t *elem_ty = ty->array.elem_ty;
            int elem_size = ty_size(elem_ty);

            // Each array element can have an initializer
            for (int i = 0; i < ty->array.cnt; ++i) {
                gen_initializer(self, elem_ty, offset, expr);
                if (expr)
                    expr = expr->next;
                offset += elem_size;
            }

            // Make sure the initializer list has no more elements
            if (expr)
                err("Trailing garbage in initializer list");
            
            break;
        }
    default:
        ASSERT_NOT_REACHED();
    }
}

static void gen_stmts(gen_t *self, stmt_t *head)
{
    for (; head; head = head->next)
        switch (head->kind) {
        case STMT_LABEL:
            ASSERT_NOT_REACHED();
            break;
        case STMT_CASE:
            ASSERT_NOT_REACHED();
            break;
        case STMT_DEFAULT:
            ASSERT_NOT_REACHED();
            break;
        case STMT_IF:
        {
            int lelse = next_label(self);
            // Generate test
            gen_bool(self, head->arg1, false, lelse);
            // Generate then
            gen_stmts(self, head->body1);
            if (head->body2) {
                // If there is an else, create a jump to the end after the then
                int lend = next_label(self);
                emit_jump(self, "jmp", lend);
                // Now we can generate the else body (with its target)
                emit_target(self, lelse);
                gen_stmts(self, head->body2);
                // Add new jump target for end
                emit_target(self, lend);
            } else {
                // If there is no else, we just use the else label as the end
                emit_target(self, lelse);
            }
            break;
        }
        case STMT_SWITCH:
            ASSERT_NOT_REACHED();
            break;
        case STMT_WHILE:
        {
            int ltest = next_label(self);
            int lbreak = next_label(self);
            // Generate test
            emit_target(self, ltest);
            gen_bool(self, head->arg1, false, lbreak);
            // Generate body
            gen_stmts(self, head->body1);
            // Jump back to test
            emit_jump(self, "jmp", ltest);
            emit_target(self, lbreak);
            break;
        }
        case STMT_DO:
        {
            int lbody = next_label(self);
            // Generate continue target and body
            emit_target(self, lbody);
            gen_stmts(self, head->body1);
            // Generate test
            gen_bool(self, head->arg1, true, lbody);
            break;
        }
        case STMT_FOR:
        {
            // Generate initialization before the loop
            if (head->arg1)
                gen_value(self, head->arg1);
            int ltest = next_label(self);
            int lbreak = next_label(self);
            // Generate test
            emit_target(self, ltest);
            if (head->arg2)
                gen_bool(self, head->arg2, false, lbreak);
            // Generate body and increment
            gen_stmts(self, head->body1);
            if (head->arg3)
                gen_value(self, head->arg3);
            // Jump back to the test
            emit_jump(self, "jmp", ltest);
            emit_target(self, lbreak);
            break;
        }
        case STMT_GOTO:
            ASSERT_NOT_REACHED();
            break;
        case STMT_CONTINUE:
            ASSERT_NOT_REACHED();
            break;
        case STMT_BREAK:
            ASSERT_NOT_REACHED();
            break;
        case STMT_RETURN:
            if (head->arg1)
                gen_value(self, head->arg1);
            break;
        case STMT_INIT:
            gen_initializer(self, head->init.ty, head->init.offset, head->arg1);
            break;
        case STMT_EVAL:
            gen_value(self, head->arg1);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
}

void gen_func(gen_t *self, sym_t *sym, int offset, stmt_t *body)
{
    // Generate entry sequence
    emit_code(self,
        "%s:\n"
        "push rbp\n"
        "mov rbp, rsp\n"
        "sub rsp, %d\n",
        sym->name,
        self->offset = align(offset, 16));

    // Setup function specific values
    self->label_cnt = 0;

    // FIXME: generate parameter spill code

    // Generate body
    gen_stmts(self, body);

    // Generate exit sequence
    emit_code(self,
        "leave\n"
        "ret\n");
}
