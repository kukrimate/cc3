// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

/** Argument to register mappings **/

static const char *barg[] = { "%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b" };
static const char *warg[] = { "%di", "%si", "%dx", "%cx", "%r8w", "%r9w" };
static const char *dwarg[] = { "%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d" };
static const char *qwarg[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

/** Context for jumps **/

typedef struct case_label case_label_t;

struct case_label {
    case_label_t *next;
    int label;
    int begin;
    int end;
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
    self->label_cnt = 0;
    string_init(&self->code);
    string_init(&self->data);
    string_init(&self->lits);
}

void gen_free(gen_t *self)
{
    string_free(&self->code);
    string_free(&self->data);
    string_free(&self->lits);
}

static int next_label(gen_t *self)
{
    return self->label_cnt++;
}

#define emit_data(self, fmt, ...)   \
    string_printf(&self->data, fmt, ## __VA_ARGS__)

#define emit_code(self, fmt, ...)   \
    string_printf(&self->code, fmt, ## __VA_ARGS__)

static int emit_str_lit(gen_t *self, const char *s)
{
    int label = next_label(self);

    // Emit name
    string_printf(&self->lits,
        ".LC%d:\n"
        "\t.byte\t",
        label);
    // Emit chars
    while (*s)
        string_printf(&self->lits, "0x%02x, ", *s++);
    // Emit NUL
    string_printf(&self->lits, "0\n");

    return label;
}

static const char *ty_data_word(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_BOOL:
        return ".byte";

    case TY_SHORT:
    case TY_USHORT:
        return ".word";

    case TY_INT:
    case TY_UINT:
        return ".long";

    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        return ".quad";

    default:
        ASSERT_NOT_REACHED();
    }
}

static void gen_static_initializer(gen_t *self, ty_t *ty, init_t *init)
{
    switch (ty->kind) {
    case TY_STRUCT:
        assert(init->kind == INIT_LIST);
        init = init->as_list;
        for (memb_t *memb = ty->as_aggregate.members; memb; memb = memb->next) {
            gen_static_initializer(self, memb->ty, init);
            init = init->next;
            if (memb->next)                             // Middle padding
                emit_data(self, "\t.align\t%d\n", ty_align(memb->next->ty));
        }
        emit_data(self, "\t.align\t%d\n", ty_align(ty));    // End padding
        break;

    case TY_UNION:
        assert(init->kind == INIT_LIST);
        init = init->as_list;
        gen_static_initializer(self, ty->as_aggregate.members->ty, init);
        emit_data(self, "\t.align\t%d\n", ty_align(ty));    // End padding
        break;

    case TY_ARRAY:
        if (init->kind == INIT_EXPR) {                  // String literal
            char *data = init->as_expr->as_str_lit.data;
            for (int i = 0; i < ty->array.cnt; ++i)
                emit_data(self, "\t.byte\t0x%02x\n", data[i]);
        } else {                                        // Initializer list
            init = init->as_list;
            for (int i = 0; i < ty->array.cnt; ++i) {
                gen_static_initializer(self, ty->array.elem_ty, init);
                init = init->next;
            }
        }
        break;

    default:
        assert(init->kind == INIT_EXPR);
        switch (init->as_expr->kind) {
        case EXPR_SYM:
            emit_data(self, "\t%s\t%s\n", ty_data_word(ty),
                init->as_expr->as_sym->asm_name);
            break;
        case EXPR_CONST:
            emit_data(self, "\t%s\t%d\n", ty_data_word(ty),
                init->as_expr->as_const.value);
            break;
        case EXPR_STR_LIT:
            emit_data(self, "\t%s\t.LC%d\n", ty_data_word(ty),
                emit_str_lit(self, init->as_expr->as_str_lit.data));
            break;
        default:
            err("Constant expression required");
        }
        break;
    }
}

void gen_static(gen_t *self, sym_t *sym, init_t *init)
{
    if (sym->kind != SYM_STATIC)
        emit_code(self, "\t.globl\t%s\n", sym->asm_name);

    emit_data(self,
        "\t.align\t%d\n"
        "%s:\n",
        ty_align(sym->ty),
        sym->asm_name);
    gen_static_initializer(self, sym->ty, init);
}

static int next_case_label(gen_t *self, jmp_ctx_t *jmp_ctx, int begin, int end)
{
    case_label_t *case_label = calloc(1, sizeof *case_label);
    if (!case_label) abort();
    case_label->label = next_label(self);
    case_label->begin = begin;
    case_label->end = end;
    *jmp_ctx->case_tail = case_label;
    jmp_ctx->case_tail = &case_label->next;
    return case_label->label;
}

static int map_goto(gen_t *self, const char *name)
{
    for (goto_label_t *cur = self->goto_labels; cur; cur = cur->next)
        if (!strcmp(cur->name, name))
            return cur->label;

    goto_label_t *new = calloc(1, sizeof *new);
    new->next = self->goto_labels;
    self->goto_labels = new;
    new->name = strdup(name);
    return (new->label = next_label(self));
}

static void emit_target(gen_t *self, int label)
{
    emit_code(self, ".L%d:\n", label);
}

static void emit_jump(gen_t *self, const char *op, int label)
{
    emit_code(self, "\t%s\t.L%d\n", op, label);
}

static void emit_push(gen_t *self, const char *reg)
{
    ++self->temp_cnt;
    emit_code(self, "\tpush\t%s\n", reg);
}

static void emit_pop(gen_t *self, const char *reg)
{
    assert(self->temp_cnt);
    --self->temp_cnt;
    emit_code(self, "\tpop\t%s\n", reg);
}

static void gen_addr(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr);
static void gen_value(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr);
static void gen_bool(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr, bool val, int label);
static void gen_stmts(gen_t *self, jmp_ctx_t *jmp_ctx, stmt_t *head);

void gen_addr(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr)
{
    switch (expr->kind) {
    case EXPR_SYM:
        {
            sym_t *sym = expr->as_sym;

            if (sym->kind == SYM_LOCAL)
                emit_code(self, "\tleaq\t-%d(%%rbp), %%rax\n",
                            self->frame_size - sym->offset);
            else
                emit_code(self, "\tleaq\t%s(%%rip), %%rax\n", sym->asm_name);
        }
        break;
    case EXPR_STR_LIT:
        emit_code(self, "\tleaq\t.LC%d(%%rip), %%rax\n",
                    emit_str_lit(self, expr->as_str_lit.data));
        break;
    case EXPR_MEMB:
        gen_addr(self, jmp_ctx, expr->as_memb.aggr);
        if (expr->as_memb.offset)
            emit_code(self, "\taddq\t$%d, %%rax\n", expr->as_memb.offset);
        break;
    case EXPR_DREF:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        break;
    case EXPR_VA_ARG:
        // First we need the address of our va_list
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        // FIXME: this only works with arguments passed in registers
        // We need to calculate the sum of gp_offset, reg_save_area
        // Then finally skip over the register we've consumed
        emit_code(self,
            "\tmovq\t%%rax, %%rcx\n"       // rcx = &va_list
            "\tmovl\t(%%rcx), %%eax\n"     // eax = va_list->gp_offset
            "\taddq\t16(%%rcx), %%rax\n"   // rax += va_list->reg_save_area
            "\taddl\t$8, (%%rcx)\n");       // va_list->gp_offstet += 8
        break;
    default:
        err("Expected lvalue expression");
    }
}

void gen_value(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr)
{
    switch (expr->kind) {
    case EXPR_SYM:
    case EXPR_STR_LIT:
    case EXPR_MEMB:
    case EXPR_DREF:
    case EXPR_VA_ARG:
        // Generate lvalue address
        gen_addr(self, jmp_ctx, expr);

        switch (expr->ty->kind) {
        // Extend smaller types to fill rax
        case TY_CHAR:
        case TY_SCHAR:
            emit_code(self, "\tmovsbq\t(%%rax), %%rax\n");
            break;
        case TY_BOOL:
        case TY_UCHAR:
            emit_code(self, "\tmovzbq\t(%%rax), %%rax\n");
            break;
        case TY_SHORT:
            emit_code(self, "\tmovswq\t(%%rax), %%rax\n");
            break;
        case TY_USHORT:
            emit_code(self, "\tmovzwq\t(%%rax), %%rax\n");
            break;
        case TY_INT:
            emit_code(self, "\tmovslq\t(%%rax), %%rax\n");
            break;
        case TY_UINT:
            // NOTE: 32-bit operand is zero extended
            emit_code(self, "\tmovl\t(%%rax), %%eax\n");
            break;

        // Native qword loads
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit_code(self, "\tmovq\t(%%rax), %%rax\n");
            break;

        case TY_FLOAT:
        case TY_DOUBLE:
        case TY_LDOUBLE:
            // FIXME: float loads
            ASSERT_NOT_REACHED();

        // Other types are left as addresses
        default:
            break;
        }

        break;

    case EXPR_CONST:
        emit_code(self, "\tmovq\t$%llu, %%rax\n", expr->as_const.value);
        break;

    case EXPR_CALL:
    {
        // Generate arguments
        int cnt = 0;
        for (expr_t *arg = expr->as_binary.rhs; arg; arg = arg->next) {
            gen_value(self, jmp_ctx, arg);
            emit_push(self, "%rax");
            ++cnt;
        }
        // Move them to registers
        assert(cnt < 6);
        while (cnt)
            emit_pop(self, qwarg[--cnt]);

        // Align frame if necessary
        if (self->temp_cnt & 1)
            emit_code(self, "\tsubq\t$8, %%rsp\n");

        // Generate call (zero rax for varargs FPU args)
        expr_t *fn = expr->as_binary.lhs;
        if (fn->kind == EXPR_SYM) {
            // Direct call
            emit_code(self,
                "\txorl\t%%eax, %%eax\n"
                "\tcall\t%s\n",
                fn->as_sym->asm_name);
        } else {
            // Indirect call
            gen_value(self, jmp_ctx, fn);
            emit_code(self,
                "\tmovq\t%%rax, %%r10\n"
                "\txorl\t%%eax, %%eax\n"
                "\tcall\t*%%r10\n");
        }

        // Remove padding if we've added some
        if (self->temp_cnt & 1)
            emit_code(self, "\taddq\t$8, %%rsp\n");

        // Extend smaller then wordsize return value
        switch (expr->ty->kind) {
        case TY_CHAR:
        case TY_SCHAR:  emit_code(self, "\tmovsbq\t%%al, %%rax\n");     break;
        case TY_UCHAR:
        case TY_BOOL:   emit_code(self, "\tmovzbq\t%%al, %%rax\n");     break;
        case TY_SHORT:  emit_code(self, "\tmovswq\t%%ax, %%rax\n");     break;
        case TY_USHORT: emit_code(self, "\tmovzwq\t%%ax, %%rax\n");     break;
        case TY_INT:    emit_code(self, "\tmovslq\t%%eax, %%rax\n");    break;
        // NOTE: 32-bit operand is zero extended
        case TY_UINT:   emit_code(self, "\tmovl\t%%eax, %%eax\n");      break;
        }

        break;
    }

    case EXPR_REF:
        gen_addr(self, jmp_ctx, expr->as_unary.arg);
        break;

    case EXPR_CAST:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        break;

    case EXPR_NEG:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        emit_code(self, "\tnegq\t%%rax\n");
        break;

    case EXPR_NOT:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        emit_code(self, "\tnotq\t%%rax\n");
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
        gen_value(self, jmp_ctx, expr->as_binary.lhs);
        emit_push(self, "%rax");
        gen_value(self, jmp_ctx, expr->as_binary.rhs);
        emit_code(self, "\tmovq\t%%rax, %%rcx\n");
        emit_pop(self, "%rax");

        switch (expr->kind) {
        case EXPR_DIV:  emit_code(self, "\txorl\t%%edx, %%edx\n\tdivq\t%%rcx\n"); break;
        case EXPR_MOD:  emit_code(self, "\txorl\t%%edx, %%edx\n\tdivq\t%%rcx\n\tmovq\t%%rdx, %%rax\n"); break;
        case EXPR_MUL:  emit_code(self, "\timulq\t%%rcx, %%rax\n"); break;
        case EXPR_ADD:  emit_code(self, "\taddq\t%%rcx, %%rax\n"); break;
        case EXPR_SUB:  emit_code(self, "\tsubq\t%%rcx, %%rax\n"); break;
        case EXPR_LSH:  emit_code(self, "\tshlq\t%%cl, %%rax\n"); break;
        case EXPR_RSH:  emit_code(self, "\tshrq\t%%cl, %%rax\n"); break;
        case EXPR_AND:  emit_code(self, "\tandq\t%%rcx, %%rax\n"); break;
        case EXPR_XOR:  emit_code(self, "\txorq\t%%rcx, %%rax\n"); break;
        case EXPR_OR:   emit_code(self, "\torq\t%%rcx, %%rax\n"); break;
        default:        ASSERT_NOT_REACHED();
        }

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
        emit_code(self, "\tmovl\t$1, %%eax\n");
        emit_jump(self, "jmp", lend);
        emit_target(self, lfalse);
        emit_code(self, "\tmov\t$0, %%eax\n");
        emit_target(self, lend);
        break;
    }

    case EXPR_AS:
    {
        gen_value(self, jmp_ctx, expr->as_binary.rhs);
        emit_push(self, "%rax");
        gen_addr(self, jmp_ctx, expr->as_binary.lhs);
        emit_code(self, "\tmovq\t%%rax, %%rcx\n");
        emit_pop(self, "%rax");

        // NOTE: do proper type conversions instead of this hackery
        // of deciding the width based on the destination type. But for
        // now this should fix NULL being truncated to 32-bits
        switch (expr->as_binary.lhs->ty->kind) {
        case TY_CHAR:
        case TY_SCHAR:
        case TY_UCHAR:
        case TY_BOOL:
            emit_code(self, "\tmovb\t%%al, (%%rcx)\n");
            break;
        case TY_SHORT:
        case TY_USHORT:
            emit_code(self, "\tmovq\t%%ax, (%%rcx)\n");
            break;
        case TY_INT:
        case TY_UINT:
            emit_code(self, "\tmovl\t%%eax, (%%rcx)\n");
            break;
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit_code(self, "\tmovq\t%%rax, (%%rcx)\n");
            break;
        case TY_STRUCT:
        case TY_UNION:  // NOTE: Struct and union assignment is a memcpy
            emit_code(self,
                "\tmovq\t%%rcx, %%rdi\n"
                "\tmovq\t%%rax, %%rsi\n"
                "\tmovabs\t$%d, %%rdx\n"
                "\tcall\tmemcpy\n",
                ty_size(expr->ty));
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        break;
    }

    case EXPR_COND:
    {
        // Compile arg1 as the condition
        int lfalse = next_label(self), lend = next_label(self);
        gen_bool(self, jmp_ctx, expr->as_trinary.arg1, false, lfalse);
        // Evaluate arg2 in the true case
        gen_value(self, jmp_ctx, expr->as_trinary.arg2);
        emit_jump(self, "jmp", lend);
        emit_target(self, lfalse);
        // Evaluate arg3 in the false case
        gen_value(self, jmp_ctx, expr->as_trinary.arg3);
        emit_target(self, lend);
        break;
    }

    case EXPR_SEQ:
        gen_value(self, jmp_ctx, expr->as_binary.lhs);
        gen_value(self, jmp_ctx, expr->as_binary.rhs);
        break;

    case EXPR_STMT:
        gen_stmts(self, jmp_ctx, expr->as_stmt);
        break;

    case EXPR_VA_START:
        // Get the address of the va_list
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        // Write gp_offset and fp_offset
        emit_code(self,
            "\tmovl\t$%d, (%%rax)\n"
            "\tmovl\t$0, 4(%%rax)\n",
            self->gp_offset);
        // Fill the pointers
        emit_code(self,
            "\tleaq\t16(%%rbp), %%rcx\n"
            "\tmovq\t%%rcx, 8(%%rax)\n");       // overflow_arg_area
        emit_code(self,
            "\tleaq\t-%d(%%rbp), %%rcx\n"
            "\tmovq\t%%rcx, 16(%%rax)\n",
            self->frame_size);                  // reg_save_area
        break;

    case EXPR_VA_END:                           // No-op on AMD64
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
        gen_value(self,jmp_ctx, expr->as_binary.lhs);
        emit_push(self, "%rax");
        gen_value(self, jmp_ctx, expr->as_binary.rhs);
        emit_code(self, "\tmovq\t%%rax, %%rcx\n");
        emit_pop(self, "%rax");
        emit_code(self, "\tcmpq\t%%rcx, %%rax\n");
        // Generate jump
        emit_jump(self, jcc(expr->kind, val), label);
        break;
    case EXPR_LNOT:
        gen_bool(self, jmp_ctx, expr->as_unary.arg, !val, label);
        break;
    case EXPR_LAND:
        if (val) {
            int lfall = next_label(self);
            gen_bool(self, jmp_ctx, expr->as_binary.lhs, false, lfall);
            gen_bool(self, jmp_ctx, expr->as_binary.rhs, true, label);
            emit_target(self, lfall);
        } else {
            gen_bool(self, jmp_ctx, expr->as_binary.lhs, false, label);
            gen_bool(self, jmp_ctx, expr->as_binary.rhs, false, label);
        }
        break;
    case EXPR_LOR:
        if (val) {
            gen_bool(self, jmp_ctx, expr->as_binary.lhs, true, label);
            gen_bool(self, jmp_ctx, expr->as_binary.rhs, true, label);
        } else {
            int lfall = next_label(self);
            gen_bool(self, jmp_ctx, expr->as_binary.lhs, true, lfall);
            gen_bool(self, jmp_ctx, expr->as_binary.rhs, false, label);
            emit_target(self, lfall);
        }
        break;
    // Test for value
    default:
        gen_value(self, jmp_ctx, expr);
        emit_code(self, "\ttestq\t%%rax, %%rax\n");
        emit_jump(self, val ? "jnz" : "jz", label);
        break;
    }
}

static void gen_write(gen_t *self, jmp_ctx_t *jmp_ctx, ty_t *ty, int offset, expr_t *expr)
{
    gen_value(self, jmp_ctx, expr);
    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_BOOL:
        emit_code(self, "\tmovb\t%%al, -%d(%%rbp)\n", self->frame_size - offset);
        break;
    case TY_SHORT:
    case TY_USHORT:
        emit_code(self, "\tmovw\t%%ax, -%d(%%rbp)\n", self->frame_size - offset);
        break;
    case TY_INT:
    case TY_UINT:
        emit_code(self, "\tmovl\t%%eax, -%d(%%rbp)\n", self->frame_size - offset);
        break;
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        emit_code(self, "\tmovq\t%%rax, -%d(%%rbp)\n", self->frame_size - offset);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

static void gen_initializer(gen_t *self, jmp_ctx_t *jmp_ctx,
    ty_t *ty, int offset, init_t *init)
{
    switch (ty->kind) {
    case TY_STRUCT:
        assert(init->kind == INIT_LIST);
        init = init->as_list;
        for (memb_t *memb = ty->as_aggregate.members; memb; memb = memb->next) {
            gen_initializer(self, jmp_ctx, memb->ty, offset + memb->offset, init);
            init = init->next;
        }
        break;
    case TY_UNION:
        assert(init->kind == INIT_LIST);
        init = init->as_list;
        gen_initializer(self, jmp_ctx, ty->as_aggregate.members->ty,
            offset + ty->as_aggregate.members->offset, init);
        break;
    case TY_ARRAY:
        if (init->kind == INIT_EXPR) {                  // String literal
            char *data = init->as_expr->as_str_lit.data;
            for (int i = 0; i < ty->array.cnt; ++i, ++offset)
                emit_code(self, "\tmovb\t$0x%02x, -%d(%%rbp)\n",
                    data[i], self->frame_size - offset);
        } else {                                        // Initializer list
            init = init->as_list;
            for (int i = 0; i < ty->array.cnt; ++i) {
                gen_initializer(self, jmp_ctx, ty->array.elem_ty, offset, init);
                init = init->next;
                offset += ty_size(ty->array.elem_ty);
            }
        }
        break;
    default:
        assert(init->kind == INIT_EXPR);
        gen_write(self, jmp_ctx, ty, offset, init->as_expr);
        break;
    }
}

static void gen_stmts(gen_t *self, jmp_ctx_t *jmp_ctx, stmt_t *head)
{
    for (; head; head = head->next)
        switch (head->kind) {
        case STMT_LABEL:
            emit_code(self, ".L%d:\n", map_goto(self, head->as_label.label));
            break;
        case STMT_CASE:
            if (!jmp_ctx->case_tail)
                err("Case only allowed inside switch");
            emit_target(self, next_case_label(self, jmp_ctx,
                head->as_case.begin, head->as_case.end));
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
            gen_bool(self, jmp_ctx, head->as_if.cond, false, lelse);

            // Generate then
            gen_stmts(self, jmp_ctx, head->as_if.then_body);

            if (head->as_if.else_body) {
                // If there is an else, create a jump to the end after the then
                int lend = next_label(self);
                emit_jump(self, "jmp", lend);
                // Now we can generate the else body (with its target)
                emit_target(self, lelse);
                gen_stmts(self, jmp_ctx, head->as_if.else_body);
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
                jmp_ctx->return_label,
                jmp_ctx->continue_label,
                next_label(self),
                &cases,
                -1,
            };

            // Jump to selection ladder
            emit_jump(self, "jmp", ladder_label);

            // Generate body
            gen_stmts(self, &switch_ctx, head->as_switch.body);
            emit_jump(self, "jmp", switch_ctx.break_label);

            // Generate selection ladder
            emit_target(self, ladder_label);
            gen_value(self, jmp_ctx, head->as_switch.cond);
            for (case_label_t *cur = cases; cur; cur = cur->next)
                if (cur->begin == cur->end) {
                    // Standard case
                    emit_code(self, "\tcmpq\t$%d, %%rax\n", cur->begin);
                    emit_jump(self, "je", cur->label);
                } else {
                    // [GNU] Case range
                    int label_skip = next_label(self);
                    emit_code(self, "\tcmpq\t$%d, %%rax\n", cur->begin);
                    emit_jump(self, "jl", label_skip);
                    emit_code(self, "\tcmpq\t$%d, %%rax\n", cur->end);
                    emit_jump(self, "jle", cur->label);
                    emit_target(self, label_skip);
                }
            if (switch_ctx.default_label != -1)
                emit_jump(self, "jmp", switch_ctx.default_label);

            emit_target(self, switch_ctx.break_label);
            break;
        }
        case STMT_WHILE:
        {
            jmp_ctx_t loop_ctx = {
                jmp_ctx->return_label,
                next_label(self),
                next_label(self),
                jmp_ctx->case_tail,
                jmp_ctx->default_label,
            };

            // Generate test
            emit_target(self, loop_ctx.continue_label);
            gen_bool(self, jmp_ctx, head->as_while.cond, false, loop_ctx.break_label);

            // Generate body
            gen_stmts(self, &loop_ctx, head->as_while.body);

            // Jump back to test
            emit_jump(self, "jmp", loop_ctx.continue_label);
            emit_target(self, loop_ctx.break_label);
            break;
        }
        case STMT_DO:
        {
            jmp_ctx_t loop_ctx = {
                jmp_ctx->return_label,
                next_label(self),
                next_label(self),
                jmp_ctx->case_tail,
                jmp_ctx->default_label,
            };

            // Generate continue target and body
            emit_target(self, loop_ctx.continue_label);
            gen_stmts(self, &loop_ctx, head->as_do.body);

            // Generate test
            gen_bool(self, jmp_ctx, head->as_do.cond, true, loop_ctx.continue_label);
            emit_target(self, loop_ctx.break_label);
            break;
        }
        case STMT_FOR:
        {
            int test_label = next_label(self);
            jmp_ctx_t loop_ctx = {
                jmp_ctx->return_label,
                next_label(self),
                next_label(self),
                jmp_ctx->case_tail,
                jmp_ctx->default_label,
            };

            // Generate initialization before the loop
            if (head->as_for.init)
                gen_value(self, jmp_ctx, head->as_for.init);

            // Generate test
            emit_target(self, test_label);
            if (head->as_for.cond)
                gen_bool(self, jmp_ctx, head->as_for.cond, false, loop_ctx.break_label);

            // Generate body and increment
            gen_stmts(self, &loop_ctx, head->as_for.body);

            emit_target(self, loop_ctx.continue_label);
            if (head->as_for.incr)
                gen_value(self, jmp_ctx, head->as_for.incr);

            // Jump back to the test
            emit_jump(self, "jmp", test_label);
            emit_target(self, loop_ctx.break_label);
            break;
        }
        case STMT_GOTO:
            emit_code(self, "\tjmp\t.L%d\n", map_goto(self, head->as_goto.label));
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
            if (head->as_return.value)
                gen_value(self, jmp_ctx, head->as_return.value);
            emit_jump(self, "jmp", jmp_ctx->return_label);
            break;
        case STMT_INIT:
            gen_initializer(self, jmp_ctx, head->as_init.sym->ty, head->as_init.sym->offset, head->as_init.value);
            break;
        case STMT_EVAL:
            gen_value(self, jmp_ctx, head->as_eval.value);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
}

static void gen_param_spill(gen_t *self, ty_t *ty, int offset, int i)
{
    assert(i < 6);

    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_BOOL:
        emit_code(self, "\tmovb\t%s, -%d(%%rbp)\n", barg[i], self->frame_size - offset);
        break;
    case TY_SHORT:
    case TY_USHORT:
        emit_code(self, "\tmovw\t%s, -%d(%%rbp)\n", warg[i], self->frame_size - offset);
        break;
    case TY_INT:
    case TY_UINT:
        emit_code(self, "\tmovl\t%s, -%d(%%rbp)\n", dwarg[i], self->frame_size - offset);
        break;
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        emit_code(self, "\tmovq\t%s, -%d(%%rbp)\n", qwarg[i], self->frame_size - offset);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

void gen_func(gen_t *self, sym_t *sym, int offset, stmt_t *body)
{
    if (sym->kind != SYM_STATIC)
        emit_code(self, "\t.globl\t%s\n", sym->asm_name);

    // Generate entry sequence
    emit_code(self,
        "%s:\n"
        "\tpush\t%%rbp\n"
        "\tmovq\t%%rsp, %%rbp\n"
        "\tsubq\t$%d, %%rsp\n",
        sym->asm_name,
        self->frame_size = align(offset, 16));

#ifdef RUNTIME_ASSERTIONS
    // Frame alignment check
    emit_code(self,
        "\tmovq\t%%rsp, %%rax\n"
        "\tandq\t$0xf, %%rax\n"
        "\tjz\t.align_ok\n"
        "\tint3\n"
        ".align_ok:\n");
#endif

    // Generate varargs register save prologue
    // NOTE: this ignores FPU registers for now
    if (sym->ty->function.var)
        for (int i = 0, offset = 0; i < 6; ++i, offset += 8)
            gen_param_spill(self, make_ty(TY_LONG), offset, i);

    // Generate parameter spill code
    param_t *param = sym->ty->function.params;
    int param_i = 0;
    for (; param; param = param->next)
        gen_param_spill(self, param->sym->ty, param->sym->offset, param_i++);

    // Starting gp_offset is after the last named parameter
    self->gp_offset = param_i * 8;

    // Fill in function state
    self->temp_cnt = 0;
    self->goto_labels = NULL;

    // Generate body
    jmp_ctx_t func_ctx = { next_label(self), -1, -1, NULL, -1 };
    gen_stmts(self, &func_ctx, body);

    // Generate exit sequence
    emit_target(self, func_ctx.return_label);
    emit_code(self,
        "\tleave\n"
        "\tret\n");
}
