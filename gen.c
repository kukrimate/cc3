#include "cc3.h"

/** Context for jumps **/

typedef struct case_label case_label_t;

struct case_label {
    case_label_t *next;
    int label;
    int val;
};

typedef struct jmp_ctx jmp_ctx_t;

struct jmp_ctx {
    int return_label;
    int continue_label;
    int break_label;
    case_label_t **case_tail;
    int default_label;
};

/** Code generator **/

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

static int next_case_label(gen_t *self, jmp_ctx_t *jmp_ctx, val_t val)
{
    case_label_t *case_label = calloc(1, sizeof *case_label);
    if (!case_label) abort();
    case_label->label = next_label(self);
    case_label->val = val;
    *jmp_ctx->case_tail = case_label;
    jmp_ctx->case_tail = &case_label->next;
    return case_label->label;
}

static void emit_target(gen_t *self, int label)
{
    emit_code(self, ".L%d:\n", label);
}

static void emit_jump(gen_t *self, const char *op, int label)
{
    emit_code(self, "%s .L%d\n", op, label);
}

static void gen_addr(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr);
static void gen_value(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr);
static void gen_bool(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr, bool val, int label);
static void gen_stmts(gen_t *self, jmp_ctx_t *jmp_ctx, stmt_t *head);

void gen_addr(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr)
{
    switch (expr->kind) {
        case EXPR_SYM:
            if (expr->sym->kind == SYM_LOCAL) {
                emit_code(self, "lea rax, [rbp - %d]\n", self->offset - expr->sym->offset);
            } else {
                emit_code(self, "mov rax, %s\n", expr->sym->name);
            }
            break;
        case EXPR_STR_LIT:
            emit_str_lit(self, expr->str);
            break;
        case EXPR_MEMB:
            gen_addr(self, jmp_ctx, expr->arg1);
            if (expr->offset)
                emit_code(self, "add rax, %d\n", expr->offset);
            break;
        case EXPR_DREF:
            gen_value(self, jmp_ctx, expr->arg1);
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

void gen_value(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr)
{
    switch (expr->kind) {
    case EXPR_SYM:
    case EXPR_STR_LIT:
    case EXPR_MEMB:
    case EXPR_DREF:
        // Generate lvalue address
        gen_addr(self, jmp_ctx, expr);

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
        static const char *qw[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

        // Generate arguments
        int cnt = 0;
        for (expr_t *arg = expr->arg2; arg; arg = arg->next) {
            gen_value(self, jmp_ctx, arg);
            emit_code(self, "push rax\n");
            ++cnt;
        }
        // Move them to registers
        assert(cnt < 6);
        while (cnt)
            emit_code(self, "pop %s\n", qw[--cnt]);

        // Generate call target
        gen_value(self, jmp_ctx, expr->arg1);
        emit_code(self, "mov r10, rax\n");

        // Do the call (zero rax for varargs)
        emit_code(self, "xor eax, eax\ncall r10\n");
        break;
    }

    case EXPR_REF:
        gen_addr(self, jmp_ctx, expr->arg1);
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
        gen_value(self, jmp_ctx, expr->arg1);
        emit_code(self, "push rax\n");
        gen_value(self, jmp_ctx, expr->arg2);
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
        gen_bool(self, jmp_ctx, expr, false, lfalse);
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
        gen_value(self, jmp_ctx, expr->arg2);
        emit_code(self, "push rax\n");
        gen_addr(self, jmp_ctx, expr->arg1);
        emit_code(self,
            "mov rcx, rax\n"
            "pop rax\n");
        switch (expr->ty->kind) {
        case TY_CHAR:
        case TY_SCHAR:
        case TY_UCHAR:
        case TY_BOOL:
            emit_code(self, "mov byte [rcx], al\n");
            break;
        case TY_SHORT:
        case TY_USHORT:
            emit_code(self, "mov word [rcx], ax\n");
            break;
        case TY_INT:
        case TY_UINT:
            emit_code(self, "mov dword [rcx], eax\n");
            break;
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit_code(self, "mov qword [rcx], rax\n");
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
        gen_bool(self, jmp_ctx, expr->arg1, false, lfalse);
        // Evaluate arg2 in the true case
        gen_value(self, jmp_ctx, expr->arg2);
        emit_jump(self, "jmp", lend);
        emit_target(self, lfalse);
        // Evaluate arg3 in the true case
        gen_value(self, jmp_ctx, expr->arg3);
        emit_target(self, lend);
        break;
    }

    case EXPR_SEQ:
        gen_value(self, jmp_ctx, expr->arg1);
        gen_value(self, jmp_ctx, expr->arg2);
        break;

    case EXPR_STMT:
        gen_stmts(self, jmp_ctx, expr->body);
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

void gen_bool(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr, bool val, int label)
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
        gen_value(self,jmp_ctx, expr->arg1);
        emit_code(self, "push rax\n");
        gen_value(self, jmp_ctx, expr->arg2);
        emit_code(self,
            "mov rcx, rax\n"
            "pop rax\n"
            "cmp rax, rcx\n");
        // Generate jump
        emit_jump(self, jcc(expr->kind, val), label);
        break;
    case EXPR_LNOT:
        gen_bool(self, jmp_ctx, expr->arg1, !val, label);
        break;
    case EXPR_LAND:
        if (val) {
            int lfall = next_label(self);
            gen_bool(self, jmp_ctx, expr->arg1, false, lfall);
            gen_bool(self, jmp_ctx, expr->arg1, true, label);
            emit_target(self, lfall);
        } else {
            gen_bool(self, jmp_ctx, expr->arg1, false, label);
            gen_bool(self, jmp_ctx, expr->arg2, false, label);
        }
        break;
    case EXPR_LOR:
        if (val) {
            gen_bool(self, jmp_ctx, expr->arg1, true, label);
            gen_bool(self, jmp_ctx, expr->arg2, true, label);
        } else {
            int lfall = next_label(self);
            gen_bool(self, jmp_ctx, expr->arg1, true, lfall);
            gen_bool(self, jmp_ctx, expr->arg1, false, label);
            emit_target(self, lfall);
        }
        break;
    // Test for value
    default:
        gen_value(self, jmp_ctx, expr);
        emit_code(self, "test rax, rax\n");
        emit_jump(self, val ? "jnz" : "jz", label);
        break;
    }
}

static void gen_init_value(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr)
{
    // Either the value itself or zero
    if (expr)
        gen_value(self, jmp_ctx, expr);
    else
        emit_code(self, "xor rax, rax\n");
}

static void gen_initializer(gen_t *self, jmp_ctx_t *jmp_ctx, ty_t *ty, int offset, expr_t *expr)
{
    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_BOOL:
        gen_init_value(self, jmp_ctx, expr);
        emit_code(self, "mov byte [rbp - %d], al\n", self->offset - offset);
        break;
    case TY_SHORT:
    case TY_USHORT:
        gen_init_value(self, jmp_ctx, expr);
        emit_code(self, "mov word [rbp - %d], ax\n", self->offset - offset);
        break;
    case TY_INT:
    case TY_UINT:
        gen_init_value(self, jmp_ctx, expr);
        emit_code(self, "mov dword [rbp - %d], eax\n", self->offset - offset);
        break;
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        gen_init_value(self, jmp_ctx, expr);
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
                gen_initializer(self, jmp_ctx, memb->ty, offset + memb->offset, expr);
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
            gen_initializer(self, jmp_ctx, memb->ty, offset + memb->offset, expr);

            // Make sure the initializer list has no more elements
            if (expr->next)
                err("Trailing garbage in initializer list");

            break;
        case TAG_ENUM:
            // Same as int
            gen_init_value(self, jmp_ctx, expr);
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
                gen_initializer(self, jmp_ctx, elem_ty, offset, expr);
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

static void gen_stmts(gen_t *self, jmp_ctx_t *jmp_ctx, stmt_t *head)
{
    for (; head; head = head->next)
        switch (head->kind) {
        case STMT_LABEL:
            emit_code(self, ".L%s:", head->label);
            break;
        case STMT_CASE:
            if (!jmp_ctx->case_tail)
                err("Case only allowed inside switch");
            emit_target(self, next_case_label(self, jmp_ctx, head->case_val));
            break;
        case STMT_DEFAULT:
            if (!jmp_ctx->case_tail)
                err("Default only allowed inside switch");
            if (jmp_ctx->default_label != -1)
                err("Only one default label allowed per switch");
            emit_target(self, jmp_ctx->default_label = next_label(self));
            break;
        case STMT_IF:
        {
            int lelse = next_label(self);

            // Generate test
            gen_bool(self, jmp_ctx, head->arg1, false, lelse);

            // Generate then
            gen_stmts(self, jmp_ctx, head->body1);

            if (head->body2) {
                // If there is an else, create a jump to the end after the then
                int lend = next_label(self);
                emit_jump(self, "jmp", lend);
                // Now we can generate the else body (with its target)
                emit_target(self, lelse);
                gen_stmts(self, jmp_ctx, head->body2);
                // Add new jump target for end
                emit_target(self, lend);
            } else {
                // If there is no else, we just use the else label as the end
                emit_target(self, lelse);
            }

            break;
        }
        case STMT_SWITCH:
        {
            case_label_t *cases;

            int ladder_label = next_label(self);
            jmp_ctx_t switch_ctx = {
                .return_label = jmp_ctx->return_label,
                .continue_label = jmp_ctx->continue_label,
                .break_label = next_label(self),
                .case_tail = &cases,
                .default_label = -1,
            };

            // Jump to selection ladder
            emit_jump(self, "jmp", ladder_label);

            // Generate body
            gen_stmts(self, &switch_ctx, head->body1);
            emit_jump(self, "jmp", switch_ctx.break_label);

            // Generate selection ladder
            emit_target(self, ladder_label);
            gen_value(self, jmp_ctx, head->arg1);
            for (case_label_t *cur = cases; cur; cur = cur->next) {
                emit_code(self, "cmp rax, %d\n", cur->val);
                emit_jump(self, "je", cur->label);
            }
            if (switch_ctx.default_label != -1)
                emit_jump(self, "jmp", switch_ctx.default_label);

            emit_target(self, switch_ctx.break_label);
            break;
        }
        case STMT_WHILE:
        {
            jmp_ctx_t loop_ctx = {
                .return_label = jmp_ctx->return_label,
                .continue_label = next_label(self),
                .break_label = next_label(self),
                .case_tail = jmp_ctx->case_tail,
                .default_label = jmp_ctx->default_label,
            };

            // Generate test
            emit_target(self, loop_ctx.continue_label);
            gen_bool(self, jmp_ctx, head->arg1, false, loop_ctx.break_label);

            // Generate body
            gen_stmts(self, &loop_ctx, head->body1);

            // Jump back to test
            emit_jump(self, "jmp", loop_ctx.continue_label);
            emit_target(self, loop_ctx.break_label);
            break;
        }
        case STMT_DO:
        {
            jmp_ctx_t loop_ctx = {
                .return_label = jmp_ctx->return_label,
                .continue_label = next_label(self),
                .break_label = next_label(self),
                .case_tail = jmp_ctx->case_tail,
                .default_label = jmp_ctx->default_label,
            };

            // Generate continue target and body
            emit_target(self, loop_ctx.continue_label);
            gen_stmts(self, &loop_ctx, head->body1);

            // Generate test
            gen_bool(self, jmp_ctx, head->arg1, true, loop_ctx.continue_label);
            emit_target(self, loop_ctx.break_label);
            break;
        }
        case STMT_FOR:
        {
            int test_label = next_label(self);
            jmp_ctx_t loop_ctx = {
                .return_label = jmp_ctx->return_label,
                .continue_label = next_label(self),
                .break_label = next_label(self),
                .case_tail = jmp_ctx->case_tail,
                .default_label = jmp_ctx->default_label,
            };

            // Generate initialization before the loop
            if (head->arg1)
                gen_value(self, jmp_ctx, head->arg1);

            // Generate test
            emit_target(self, test_label);
            if (head->arg2)
                gen_bool(self, jmp_ctx, head->arg2, false, loop_ctx.break_label);

            // Generate body and increment
            gen_stmts(self, &loop_ctx, head->body1);

            emit_target(self, loop_ctx.continue_label);
            if (head->arg3)
                gen_value(self, jmp_ctx, head->arg3);

            // Jump back to the test
            emit_jump(self, "jmp", test_label);
            emit_target(self, loop_ctx.break_label);
            break;
        }
        case STMT_GOTO:
            emit_code(self, "jmp .L%s\n", head->label);
            break;
        case STMT_CONTINUE:
            if (jmp_ctx->continue_label == -1)
                err("Continue statement only allowed inside loops");
            emit_jump(self, "jmp", jmp_ctx->continue_label);
            break;
        case STMT_BREAK:
            if (jmp_ctx->break_label == -1)
                err("break statement only allowed inside loops or switch");
            emit_jump(self, "jmp", jmp_ctx->break_label);
            break;
        case STMT_RETURN:
            if (head->arg1)
                gen_value(self, jmp_ctx, head->arg1);
            emit_jump(self, "jmp", jmp_ctx->return_label);
            break;
        case STMT_INIT:
            gen_initializer(self, jmp_ctx, head->init.ty, head->init.offset, head->arg1);
            break;
        case STMT_EVAL:
            gen_value(self, jmp_ctx, head->arg1);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
}

static void gen_param_spill(gen_t *self, int offset, ty_t *ty, int i)
{
    assert(i < 6);

    static const char *byte[] = { "dil", "sil", "dl", "cl", "r8b", "r9b" };
    static const char *word[] = { "di", "si", "dx", "cx", "r8w", "r9w" };
    static const char *dw[] = { "edi", "esi", "edx", "ecx", "r8d", "r9d" };
    static const char *qw[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_BOOL:
        emit_code(self, "mov byte [rbp - %d], %s\n", self->offset - offset, byte[i]);
        break;
    case TY_SHORT:
    case TY_USHORT:
        emit_code(self, "mov word [rbp - %d], %s\n", self->offset - offset, word[i]);
        break;
    case TY_INT:
    case TY_UINT:
        emit_code(self, "mov dword [rbp - %d], %s\n", self->offset - offset, dw[i]);
        break;
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        emit_code(self, "mov qword [rbp - %d], %s\n", self->offset - offset, qw[i]);
        break;
    case TY_TAG:
        switch (ty->tag->kind) {
        case TAG_STRUCT:
            ASSERT_NOT_REACHED();
            break;
        case TAG_UNION:
            ASSERT_NOT_REACHED();
            break;
        case TAG_ENUM:
            emit_code(self, "mov dword [rbp - %d], %s\n", self->offset - offset, dw[i]);
            break;
        }
        break;
    case TY_ARRAY:
        ASSERT_NOT_REACHED();
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

    // Generate parameter spill code
    int param_i = 0;
    for (ty_t *param = sym->ty->function.param_tys; param; param = param->next) {
        gen_param_spill(self, param->sym->offset, param, param_i++);
    }

    // Generate body
    jmp_ctx_t func_ctx = {
        .return_label = next_label(self),
        .continue_label = -1,
        .break_label = -1,
        .case_tail = NULL,
        .default_label = -1,
    };
    gen_stmts(self, &func_ctx, body);

    // Generate exit sequence
    emit_target(self, func_ctx.return_label);
    emit_code(self,
        "leave\n"
        "ret\n");
}
