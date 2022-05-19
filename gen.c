// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

/** Argument to register mappings **/

static const char *barg[] = { "%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b" };
static const char *warg[] = { "%di", "%si", "%dx", "%cx", "%r8w", "%r9w" };
static const char *dwarg[] = { "%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d" };
static const char *qwarg[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

#define VA_REGSAVE_SIZE 48

/** Context for jumps **/

typedef struct {
    int label;
    int begin;
    int end;
} case_t;

VEC_DEF(case_vec, case_t)
VEC_GEN(case_vec, case_t)

typedef struct {
    int return_label;
    int continue_label;
    int break_label;
    case_vec_t *cases;
    int default_label;
} jmp_ctx_t;

/** Code generator **/

void gen_init(gen_t *self, int out_fd)
{
    self->out_fd = out_fd;
    self->label_cnt = 0;
    map_init(&self->lits);
    map_init(&self->gotos);
}

void gen_free(gen_t *self)
{
    map_free(&self->lits);
    map_free(&self->gotos);
}

static int next_label(gen_t *self)
{
    return self->label_cnt++;
}

#define emit(self, fmt, ...)   \
    dprintf(self->out_fd, fmt, ## __VA_ARGS__)

static int lit_to_label(gen_t *self, const char *data)
{
    bool found;
    entry_t *entry = map_find_or_insert(&self->lits, data, &found);

    if (found)
        return entry->as_int;
    else
        return entry->as_int = next_label(self);
}

static void gen_const_addr(gen_t *self, expr_t *expr, int offset)
{
    switch (expr->kind) {
    case EXPR_SYM:
        emit(self, "\t.quad\t%s + %d\n",
                expr->as_sym->asm_name, offset);
        break;
    case EXPR_STR:
        emit(self, "\t.quad\t.LC%d + %d\n",
                lit_to_label(self, expr->as_str.data), offset);
        break;
    case EXPR_MEMB:
        gen_const_addr(self, expr->as_memb.aggr, offset + expr->as_memb.offset);
        break;
    default:
        err("Constant lvalue required");
    }
}

static void gen_const_expr(gen_t *self, expr_t *expr)
{
    switch (expr->kind) {

    // Numeric constants are self-explanatory
    case EXPR_CONST:
        switch (expr->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
        case TY_SCHAR:
        case TY_UCHAR:
            emit(self, "\t.byte\t%lld\n", expr->as_const.value);
            break;
        case TY_SHORT:
        case TY_USHORT:
            emit(self, "\t.word\t%lld\n", expr->as_const.value);
            break;
        case TY_INT:
        case TY_UINT:
            emit(self, "\t.long\t%lld\n", expr->as_const.value);
            break;
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit(self, "\t.quad\t%lld\n", expr->as_const.value);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        break;

    // The address of link time known objects can also be used as a constant.
    case EXPR_REF:
        gen_const_addr(self, expr->as_unary.arg, 0);
        break;

    // arrays and function designators get implicitly prefixed with &
    case EXPR_SYM:
    case EXPR_STR:
    case EXPR_MEMB:
    case EXPR_DREF:
        if (expr->ty->kind == TY_ARRAY || expr->ty->kind == TY_FUNCTION) {
            gen_const_addr(self, expr, 0);
            break;
        }

        FALLTHROUGH;

    default:
        err("Constant value required");
    }
}

static void gen_static_initializer(gen_t *self, ty_t *ty, init_t *init)
{
    switch (ty->kind) {
    case TY_STRUCT:
        assert(init->kind == INIT_LIST);
        int i = 0;
        for (memb_t *memb = ty->as_aggregate.members; memb; memb = memb->next) {
            gen_static_initializer(self, memb->ty, VEC_AT(&init->as_list, i++));
            if (memb->next)                             // Middle padding
                emit(self, "\t.align\t%d\n", memb->next->ty->align);
        }

        emit(self, "\t.align\t%d\n", ty->align);        // End padding
        break;

    case TY_UNION:
        assert(init->kind == INIT_LIST);
        gen_static_initializer(self,
            ty->as_aggregate.members->ty,
            VEC_AT(&init->as_list, 0));
        emit(self, "\t.align\t%d\n", ty->align);        // End padding
        break;

    case TY_ARRAY:
        assert(init->kind == INIT_LIST);
        for (int i = 0; i < ty->array.cnt; ++i)
            gen_static_initializer(self,
                ty->array.elem_ty,
                VEC_AT(&init->as_list, i));
        break;

    default:
        assert(init->kind == INIT_EXPR);
        gen_const_expr(self, init->as_expr);
        break;
    }
}

void gen_static(gen_t *self, sym_t *sym, init_t *init)
{
    emit(self, "\t.section\t.data\n");
    if (sym->kind != SYM_STATIC)
        emit(self, "\t.globl\t%s\n", sym->asm_name);

    emit(self,
        "\t.align\t%d\n"
        "%s:\n",
        sym->ty->align,
        sym->asm_name);

    gen_static_initializer(self, sym->ty, init);
}

static int next_case_label(gen_t *self, jmp_ctx_t *jmp_ctx, int begin, int end)
{
    // Verify that it doesn't overlap with any previous cases
    VEC_FOREACH(jmp_ctx->cases, cur)
        if ((cur->begin >= begin && cur->begin <= end)
                || (cur->end >= begin && cur->end <= end))
            err("Overlapping case statements");

    // If there was no overlap we can add a new case label
    case_t *cur = case_vec_push(jmp_ctx->cases);
    cur->begin = begin;
    cur->end = end;
    return cur->label = next_label(self);
}

static int map_goto(gen_t *self, const char *name)
{
    bool found;
    entry_t *entry = map_find_or_insert(&self->gotos, name, &found);

    if (found)
        return entry->as_int;
    else
        return entry->as_int = next_label(self);
}

static void emit_target(gen_t *self, int label)
{
    emit(self, ".L%d:\n", label);
}

static void emit_jump(gen_t *self, const char *op, int label)
{
    emit(self, "\t%s\t.L%d\n", op, label);
}

static void emit_push(gen_t *self, const char *reg)
{
    ++self->temp_cnt;
    emit(self, "\tpush\t%s\n", reg);
}

static void emit_pop(gen_t *self, const char *reg)
{
    assert(self->temp_cnt);
    --self->temp_cnt;
    emit(self, "\tpop\t%s\n", reg);
}

static expr_t *match_unary(expr_t *expr, int kind1, int kind2)
{
    if (expr->kind != kind1)
        return NULL;
    if ((expr = expr->as_unary.arg)->kind != kind2)
        return NULL;
    return expr;
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
                emit(self, "\tleaq\t%d(%%rbp), %%rax\n", sym->offset);
            else
                emit(self, "\tmovq\t%s@GOTPCREL(%%rip), %%rax\n", sym->asm_name);
        }
        break;
    case EXPR_STR:
        emit(self, "\tmovq\t.LC%d@GOTPCREL(%%rip), %%rax\n",
                    lit_to_label(self, expr->as_str.data));
        break;
    case EXPR_MEMB:
        gen_addr(self, jmp_ctx, expr->as_memb.aggr);
        if (expr->as_memb.offset)
            emit(self, "\taddq\t$%d, %%rax\n", expr->as_memb.offset);
        break;
    case EXPR_DREF:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        break;
    case EXPR_VA_ARG:
        gen_value(self, jmp_ctx, expr->as_unary.arg);

        emit(self,
            "\tmovq\t%%rax, %%rcx\n"        // rcx = &va_list
            "\tmovl\t(%%rcx), %%eax\n");    // eax = va_list->gp_offset
        // Check if we need to look at overflow arg area
        int loflo = next_label(self), lend = next_label(self);
        emit(self,
            "\tcmpl\t$48, %%eax\n"          // if eax == 48 then goto oflo;
            "\tje\t.L%d\n", loflo);
        // Pull argument from register save area
        emit(self,
            "\taddq\t16(%%rcx), %%rax\n"    // rax += va_list->reg_save_area
            "\taddl\t$8, (%%rcx)\n"         // va_list->gp_offset += 8
            "\tjmp\t.L%d\n", lend);         // goto end;
        // Pull argument from stack
        emit(self,
            ".L%d:\n"
            "\tmovq\t8(%%rcx), %%rax\n"     // rax = va_list->overflow_arg_area
            "\taddq\t$%d, 8(%%rcx)\n"       // va_list->overflow_arg_area += ty_size
            ".L%d:\n",
            loflo,
            align(expr->ty->size, 8),
            lend);

        break;
    default:
        err("Expected lvalue expression");
    }
}

static void gen_call(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *func, expr_vec_t *args)
{
    // Calculate the number of overflow arguments
    int oflo_cnt;
    if (args->length > 6)
        oflo_cnt = args->length - 6;
    else
        oflo_cnt = 0;

    // Make sure the stack is always 16 bytes aligned before the call
    if ((self->temp_cnt + oflo_cnt) & 1) {
        emit_push(self, "%rax");
        ++oflo_cnt;
    }

    // Push arguments on the stack in reverse order
    VEC_FOREACH_REV(args, arg) {
        gen_value(self, jmp_ctx, *arg);
        emit_push(self, "%rax");
    }
    // Pop off the ones that belong in registers
    for (int i = 0; i < 6 && i < args->length; ++i)
        emit_pop(self, qwarg[i]);

    // Generate call (zero rax for varargs FPU args)
    ty_t *func_ty = func->ty->pointer.base_ty;
    expr_t *sym = match_unary(func, EXPR_REF, EXPR_SYM);

    if (sym) {
        if (func_ty->function.var)
            emit(self, "\txorl\t%%eax, %%eax\n");
        emit(self, "\tcall\t%s@PLT\n", sym->as_sym->asm_name);
    } else {
        gen_value(self, jmp_ctx, func);
        emit(self, "\tmovq\t%%rax, %%r10\n");
        if (func_ty->function.var)
            emit(self, "\txorl\t%%eax, %%eax\n");
        emit(self, "\tcall\t*%%r10\n");
    }

    // After the call remove overflow arguments and any padding
    if (oflo_cnt) {
        emit(self, "\taddq\t$%d, %%rsp\n", oflo_cnt * 8);
        self->temp_cnt -= oflo_cnt;
    }
}

void gen_value(gen_t *self, jmp_ctx_t *jmp_ctx, expr_t *expr)
{
    switch (expr->kind) {
    case EXPR_SYM:
    case EXPR_STR:
    case EXPR_MEMB:
    case EXPR_DREF:
    case EXPR_VA_ARG:
        // Generate lvalue address
        gen_addr(self, jmp_ctx, expr);

        switch (expr->ty->kind) {
        // Extend smaller types to fill rax
        case TY_CHAR:
        case TY_SCHAR:
            emit(self, "\tmovsbq\t(%%rax), %%rax\n");
            break;
        case TY_BOOL:
        case TY_UCHAR:
            emit(self, "\tmovzbq\t(%%rax), %%rax\n");
            break;
        case TY_SHORT:
            emit(self, "\tmovswq\t(%%rax), %%rax\n");
            break;
        case TY_USHORT:
            emit(self, "\tmovzwq\t(%%rax), %%rax\n");
            break;
        case TY_INT:
            emit(self, "\tmovslq\t(%%rax), %%rax\n");
            break;
        case TY_UINT:
            // NOTE: 32-bit operand is zero extended
            emit(self, "\tmovl\t(%%rax), %%eax\n");
            break;

        // Native qword loads
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit(self, "\tmovq\t(%%rax), %%rax\n");
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
        emit(self, "\tmovq\t$%llu, %%rax\n", expr->as_const.value);
        break;

    case EXPR_CALL:
        gen_call(self, jmp_ctx, expr->as_call.func, &expr->as_call.args);
        // Extend smaller then wordsize return value
        switch (expr->ty->kind) {
        case TY_CHAR:
        case TY_SCHAR:  emit(self, "\tmovsbq\t%%al, %%rax\n");     break;
        case TY_BOOL:
        case TY_UCHAR:  emit(self, "\tmovzbq\t%%al, %%rax\n");     break;
        case TY_SHORT:  emit(self, "\tmovswq\t%%ax, %%rax\n");     break;
        case TY_USHORT: emit(self, "\tmovzwq\t%%ax, %%rax\n");     break;
        case TY_INT:    emit(self, "\tmovslq\t%%eax, %%rax\n");    break;
        // NOTE: 32-bit operand is zero extended
        case TY_UINT:   emit(self, "\tmovl\t%%eax, %%eax\n");      break;
        }
        break;

    case EXPR_REF:
        gen_addr(self, jmp_ctx, expr->as_unary.arg);
        break;

    case EXPR_CAST:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        break;

    case EXPR_NEG:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        emit(self, "\tnegq\t%%rax\n");
        break;

    case EXPR_NOT:
        gen_value(self, jmp_ctx, expr->as_unary.arg);
        emit(self, "\tnotq\t%%rax\n");
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
        gen_value(self, jmp_ctx, expr->as_binary.rhs);
        emit_push(self, "%rax");
        gen_value(self, jmp_ctx, expr->as_binary.lhs);
        emit_pop(self, "%rcx");

        switch (expr->kind) {
        case EXPR_DIV:  emit(self, "\txorl\t%%edx, %%edx\n\tdivq\t%%rcx\n"); break;
        case EXPR_MOD:  emit(self, "\txorl\t%%edx, %%edx\n\tdivq\t%%rcx\n\tmovq\t%%rdx, %%rax\n"); break;
        case EXPR_MUL:  emit(self, "\timulq\t%%rcx, %%rax\n"); break;
        case EXPR_ADD:  emit(self, "\taddq\t%%rcx, %%rax\n"); break;
        case EXPR_SUB:  emit(self, "\tsubq\t%%rcx, %%rax\n"); break;
        case EXPR_LSH:  emit(self, "\tshlq\t%%cl, %%rax\n"); break;
        case EXPR_RSH:  emit(self, "\tshrq\t%%cl, %%rax\n"); break;
        case EXPR_AND:  emit(self, "\tandq\t%%rcx, %%rax\n"); break;
        case EXPR_XOR:  emit(self, "\txorq\t%%rcx, %%rax\n"); break;
        case EXPR_OR:   emit(self, "\torq\t%%rcx, %%rax\n"); break;
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
        emit(self, "\tmovl\t$1, %%eax\n");
        emit_jump(self, "jmp", lend);
        emit_target(self, lfalse);
        emit(self, "\tmov\t$0, %%eax\n");
        emit_target(self, lend);
        break;
    }

    case EXPR_AS:
    {
        gen_addr(self, jmp_ctx, expr->as_binary.lhs);
        emit_push(self, "%rax");
        gen_value(self, jmp_ctx, expr->as_binary.rhs);
        emit_pop(self, "%rcx");

        switch (expr->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
        case TY_SCHAR:
        case TY_UCHAR:
            emit(self, "\tmovb\t%%al, (%%rcx)\n");
            break;
        case TY_SHORT:
        case TY_USHORT:
            emit(self, "\tmovq\t%%ax, (%%rcx)\n");
            break;
        case TY_INT:
        case TY_UINT:
            emit(self, "\tmovl\t%%eax, (%%rcx)\n");
            break;
        case TY_LONG:
        case TY_ULONG:
        case TY_LLONG:
        case TY_ULLONG:
        case TY_POINTER:
            emit(self, "\tmovq\t%%rax, (%%rcx)\n");
            break;
        case TY_STRUCT:
        case TY_UNION:  // NOTE: Struct and union assignment is a memcpy
            emit(self,
                "\tmovq\t%%rcx, %%rdi\n"
                "\tmovq\t%%rax, %%rsi\n"
                "\tmovq\t$%d, %%rdx\n"
                "\tcall\tmemcpy\n",
                expr->ty->size);
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
        emit(self,
            "\tmovl\t$%d, (%%rax)\n"
            "\tmovl\t$%d, 4(%%rax)\n",
            self->gp_offset,
            self->fp_offset);
        // Fill the pointers
        emit(self,
            "\tleaq\t16(%%rbp), %%rcx\n"
            "\tmovq\t%%rcx, 8(%%rax)\n");       // overflow_arg_area
        emit(self,
            "\tleaq\t%d(%%rbp), %%rcx\n"
            "\tmovq\t%%rcx, 16(%%rax)\n",
            -VA_REGSAVE_SIZE);                  // reg_save_area
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
        emit(self, "\tmovq\t%%rax, %%rcx\n");
        emit_pop(self, "%rax");
        emit(self, "\tcmpq\t%%rcx, %%rax\n");
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
        emit(self, "\ttestq\t%%rax, %%rax\n");
        emit_jump(self, val ? "jnz" : "jz", label);
        break;
    }
}

static void gen_write(gen_t *self, jmp_ctx_t *jmp_ctx, ty_t *ty, int offset, expr_t *expr)
{
    gen_value(self, jmp_ctx, expr);
    switch (ty->kind) {
    case TY_BOOL:
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
        emit(self, "\tmovb\t%%al, %d(%%rbp)\n", offset);
        break;
    case TY_SHORT:
    case TY_USHORT:
        emit(self, "\tmovw\t%%ax, %d(%%rbp)\n", offset);
        break;
    case TY_INT:
    case TY_UINT:
        emit(self, "\tmovl\t%%eax, %d(%%rbp)\n", offset);
        break;
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        emit(self, "\tmovq\t%%rax, %d(%%rbp)\n", offset);
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
        int i = 0;
        for (memb_t *memb = ty->as_aggregate.members; memb; memb = memb->next) {
            gen_initializer(self, jmp_ctx, memb->ty,
                offset + memb->offset, VEC_AT(&init->as_list, i++));
        }
        break;
    case TY_UNION:
        assert(init->kind == INIT_LIST);
        gen_initializer(self, jmp_ctx, ty->as_aggregate.members->ty,
            offset + ty->as_aggregate.members->offset,
            VEC_AT(&init->as_list, 0));
        break;
    case TY_ARRAY:
        assert(init->kind == INIT_LIST);
        for (int i = 0; i < ty->array.cnt; ++i) {
            gen_initializer(self, jmp_ctx, ty->array.elem_ty,
                offset, VEC_AT(&init->as_list, i));
            offset += ty->array.elem_ty->size;
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
            emit(self, ".L%d:\n", map_goto(self, head->as_label.label));
            break;
        case STMT_CASE:
            if (!jmp_ctx->cases)
                err("Case only allowed inside switch");
            emit_target(self, next_case_label(self, jmp_ctx,
                head->as_case.begin, head->as_case.end));
            break;
        case STMT_DEFAULT:
            if (!jmp_ctx->cases)
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
            case_vec_t cases;
            case_vec_init(&cases);

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

            VEC_FOREACH(&cases, cur)
                if (cur->begin == cur->end) {
                    // Standard case
                    emit(self, "\tcmpq\t$%d, %%rax\n", cur->begin);
                    emit_jump(self, "je", cur->label);
                } else {
                    // [GNU] Case range
                    int label_skip = next_label(self);
                    emit(self, "\tcmpq\t$%d, %%rax\n", cur->begin);
                    emit_jump(self, "jl", label_skip);
                    emit(self, "\tcmpq\t$%d, %%rax\n", cur->end);
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
                jmp_ctx->cases,
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
                jmp_ctx->cases,
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
                jmp_ctx->cases,
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
            emit(self, "\tjmp\t.L%d\n", map_goto(self, head->as_goto.label));
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
        case STMT_DECL:
        {
            sym_t *sym = head->as_decl.sym;
            // Allocate local variable
            self->offset += sym->ty->size;
            sym->offset = -(self->offset = align(self->offset, sym->ty->align));
            // Generate initializer
            if (head->as_decl.has_init)
                gen_initializer(self, jmp_ctx,
                    sym->ty, sym->offset, &head->as_decl.init);
            break;
        }
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
    case TY_BOOL:
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
        emit(self, "\tmovb\t%s, %d(%%rbp)\n", barg[i], offset);
        break;
    case TY_SHORT:
    case TY_USHORT:
        emit(self, "\tmovw\t%s, %d(%%rbp)\n", warg[i], offset);
        break;
    case TY_INT:
    case TY_UINT:
        emit(self, "\tmovl\t%s, %d(%%rbp)\n", dwarg[i], offset);
        break;
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_POINTER:
        emit(self, "\tmovq\t%s, %d(%%rbp)\n", qwarg[i], offset);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

void gen_func(gen_t *self, sym_t *sym, stmt_t *body)
{
    emit(self, "\t.text\n");
    if (sym->kind != SYM_STATIC)
        emit(self, "\t.globl\t%s\n", sym->asm_name);

    // Generate entry sequence
    emit(self,
        "%s:\n"
        "\tpush\t%%rbp\n"
        "\tmovq\t%%rsp, %%rbp\n"
        "\tsubq\t$%s_fs, %%rsp\n",
        sym->asm_name,
        sym->asm_name);

#ifdef RUNTIME_ASSERTIONS
    int lframe_ok = next_label(self);
    // Frame alignment check
    emit(self,
        "\tmovq\t%%rsp, %%rax\n"
        "\tandq\t$0xf, %%rax\n"
        "\tjz\t.L%d\n"
        "\tint3\n"
        ".L%d:\n", lframe_ok, lframe_ok);
#endif

    // Start with an empty frame
    self->offset = 0;

    // Allocate varargs spill space
    if (sym->ty->function.var)
        self->offset += VA_REGSAVE_SIZE;

    // Spill named parameters
    int gp = 0, oflo = 16;

    for (param_t *param = sym->ty->function.params; param; param = param->next)
        if (gp < 6) {
            self->offset += param->ty->size;
            self->offset = align(self->offset, param->ty->align);
            gen_param_spill(self,
                param->sym->ty,
                param->sym->offset = -self->offset,
                gp++);
        } else {
            int palign = param->ty->align;
            param->sym->offset =
                (oflo = align(oflo, palign < 8 ? 8 : palign));
            oflo += param->ty->size;
        }

    self->gp_offset = gp * 8;
    self->fp_offset = 48;

    // Spill the rest of the registers for varargs registers
    if (sym->ty->function.var)
        for (; gp < 6; ++gp)
            emit(self, "\tmovq\t%s, %d(%%rbp)\n", qwarg[gp], -VA_REGSAVE_SIZE + gp * 8);

    self->temp_cnt = 0;
    map_clear(&self->gotos);

    // Generate body
    jmp_ctx_t func_ctx = { next_label(self), -1, -1, NULL, -1 };
    gen_stmts(self, &func_ctx, body);

    // Generate exit sequence
    emit_target(self, func_ctx.return_label);
    emit(self,
        "\tleave\n"
        "\tret\n");

    // Provide stackframe size
    emit(self, "\t.set\t%s_fs, %d\n", sym->asm_name, align(self->offset, 16));
}

void gen_lits(gen_t *self)
{
    emit(self, "\t.section\t.rodata\n");

    MAP_ITER(&self->lits, entry) {
        // Emit name
        emit(self,
            ".LC%d:\n"
            "\t.byte\t", entry->as_int);
        // Emit chars
        for (const char *data = entry->key; *data; ++data)
            emit(self, "0x%02x, ", *data);
        // Emit NUL
        emit(self, "0\n");
    }
}
