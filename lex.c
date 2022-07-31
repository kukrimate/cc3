// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

static bool have_strtab = false;
static int_map_t strtab = {0};

static void strtab_init(void)
{
    int_map_init(&strtab);

    // Standard keywords
    int_map_find_or_insert(&strtab, "auto", NULL)->value = TK_AUTO;
    int_map_find_or_insert(&strtab, "break", NULL)->value = TK_BREAK;
    int_map_find_or_insert(&strtab, "case", NULL)->value = TK_CASE;
    int_map_find_or_insert(&strtab, "char", NULL)->value = TK_CHAR;
    int_map_find_or_insert(&strtab, "const", NULL)->value = TK_CONST;
    int_map_find_or_insert(&strtab, "continue", NULL)->value = TK_CONTINUE;
    int_map_find_or_insert(&strtab, "default", NULL)->value = TK_DEFAULT;
    int_map_find_or_insert(&strtab, "do", NULL)->value = TK_DO;
    int_map_find_or_insert(&strtab, "double", NULL)->value = TK_DOUBLE;
    int_map_find_or_insert(&strtab, "else", NULL)->value = TK_ELSE;
    int_map_find_or_insert(&strtab, "enum", NULL)->value = TK_ENUM;
    int_map_find_or_insert(&strtab, "extern", NULL)->value = TK_EXTERN;
    int_map_find_or_insert(&strtab, "float", NULL)->value = TK_FLOAT;
    int_map_find_or_insert(&strtab, "for", NULL)->value = TK_FOR;
    int_map_find_or_insert(&strtab, "goto", NULL)->value = TK_GOTO;
    int_map_find_or_insert(&strtab, "if", NULL)->value = TK_IF;
    int_map_find_or_insert(&strtab, "inline", NULL)->value = TK_INLINE;
    int_map_find_or_insert(&strtab, "int", NULL)->value = TK_INT;
    int_map_find_or_insert(&strtab, "long", NULL)->value = TK_LONG;
    int_map_find_or_insert(&strtab, "register", NULL)->value = TK_REGISTER;
    int_map_find_or_insert(&strtab, "restrict", NULL)->value = TK_RESTRICT;
    int_map_find_or_insert(&strtab, "return", NULL)->value = TK_RETURN;
    int_map_find_or_insert(&strtab, "short", NULL)->value = TK_SHORT;
    int_map_find_or_insert(&strtab, "signed", NULL)->value = TK_SIGNED;
    int_map_find_or_insert(&strtab, "sizeof", NULL)->value = TK_SIZEOF;
    int_map_find_or_insert(&strtab, "static", NULL)->value = TK_STATIC;
    int_map_find_or_insert(&strtab, "struct", NULL)->value = TK_STRUCT;
    int_map_find_or_insert(&strtab, "switch", NULL)->value = TK_SWITCH;
    int_map_find_or_insert(&strtab, "typedef", NULL)->value = TK_TYPEDEF;
    int_map_find_or_insert(&strtab, "union", NULL)->value = TK_UNION;
    int_map_find_or_insert(&strtab, "unsigned", NULL)->value = TK_UNSIGNED;
    int_map_find_or_insert(&strtab, "void", NULL)->value = TK_VOID;
    int_map_find_or_insert(&strtab, "volatile", NULL)->value = TK_VOLATILE;
    int_map_find_or_insert(&strtab, "while", NULL)->value = TK_WHILE;
    int_map_find_or_insert(&strtab, "_Bool", NULL)->value = TK_BOOL;
    int_map_find_or_insert(&strtab, "_Complex", NULL)->value = TK_COMPLEX;
    int_map_find_or_insert(&strtab, "_Imaginary", NULL)->value = TK_IMAGINARY;

    // Extensions
    int_map_find_or_insert(&strtab, "__inline", NULL)->value = TK_INLINE;
    int_map_find_or_insert(&strtab, "__typeof__", NULL)->value = TK_TYPEOF;
    int_map_find_or_insert(&strtab, "__builtin_va_list", NULL)->value = TK_VA_LIST;
    int_map_find_or_insert(&strtab, "__builtin_va_start", NULL)->value = TK_VA_START;
    int_map_find_or_insert(&strtab, "__builtin_va_end", NULL)->value = TK_VA_END;
    int_map_find_or_insert(&strtab, "__builtin_va_arg", NULL)->value = TK_VA_ARG;
}

static int_entry_t *strtab_intern(const char *spelling)
{
    bool found;
    int_entry_t *entry = int_map_find_or_insert(&strtab, spelling, &found);

    if (!found) {
        entry->key = strdup(spelling);
        entry->value = -1;
    }

    return entry;
}

void lex_init(lexer_t *self, int in_fd)
{
    if (!have_strtab) {
        have_strtab = true;
        strtab_init();
    }

    self->in_fd = in_fd;

    self->cur = self->buf;
    self->end = self->buf;

    self->line = 1;
    self->col = 1;

    string_init(&self->spelling);
    string_init(&self->decode);
}

void lex_free(lexer_t *self)
{
    string_free(&self->spelling);
    string_free(&self->decode);
}

static int peek(lexer_t *self, int i)
{
    // Return character if in bounds
    if (self->cur + i < self->end)
        return self->cur[i];

    // Copy chars to the start of the buffer
    char *oldcur = self->cur;
    char *oldend = self->end;
    self->cur = self->buf;
    self->end = self->buf;
    while (oldend > oldcur) {
        *self->end++ = *--oldend;
    }

    // Fill buffer with characters
    self->end += read(self->in_fd, self->end, LEX_BUF_SZ / 2);

    // Return character if we read enough
    if (self->cur + i < self->end)
        return self->cur[i];

    // Otherwise we know this was EOF
    return EOF;
}

static int eat(lexer_t *self)
{
    int ch = *self->cur++;

    // Handle newline
    if (ch == '\n') {
        ++self->line;
        self->col = 0;
    } else {
        ++self->col;
    }

    // Add character to spelling
    string_push(&self->spelling, ch);

    return ch;
}

static int input(lexer_t *self)
{
    int ch = peek(self, 0);
    // Increment current position if not at EOF
    if (ch != EOF)
        eat(self);
    // Return char we've just read
    return ch;
}

static bool match(lexer_t *self, int want)
{
    int ch = peek(self, 0);
    // Increment position if it's a match
    if (ch == want) {
        eat(self);
        return true;
    }
    return false;
}

static int unescape(lexer_t *self)
{
    int val = input(self);

    switch (val) {
    // Single char
    case '\'':
    case '"':
    case '?':
    case '\\':  return val;
    case 'a':   return '\a';
    case 'b':   return '\b';
    case 'f':   return '\f';
    case 'n':   return '\n';
    case 'r':   return '\r';
    case 't':   return '\t';
    case 'v':   return '\v';
    // Octal
    case '0' ... '7':
        val -= '0';
        for (int ch;; eat(self))
            switch ((ch = peek(self, 0))) {
            case '0' ... '7':
                val = val << 3 | (ch - '0');
                break;
            default:
                return val;
            }
    // Hex
    case 'x':
        for (int ch;; eat(self))
            switch ((ch = peek(self, 0))) {
            case '0' ... '9':
                val = val << 4 | (ch - '0');
                break;
            case 'a' ... 'f':
                val = val << 4 | (ch - 'a' + 0xa);
                break;
            case 'A' ... 'F':
                val = val << 4 | (ch - 'A' + 0xa);
                break;
            default:
                return val;
            }
    default:
        err("Invalid escape sequence %c", val);
    }
}

static void int_suffix(lexer_t *self)
{
    // FIXME: this is not really correct
    while (match(self, 'u') || match(self, 'U')
            || match(self, 'l') || match(self, 'L'))
        ;
}

static bool iswhite(int ch)
{
    switch (ch) {
    case ' ':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
    case '\v':
        return true;
    default:
        return false;
    }
}

void lex_next(lexer_t *self, tk_t *tk)
{
    int ch;

retry:
    // Save token starting position
    tk->line = self->line;
    tk->col = self->col;

    // Clear token spelling
    string_clear(&self->spelling);

    switch ((ch = input(self))) {
    // End of file
    case EOF:
        tk->kind = TK_EOF;
        break;
    // Whitespace
    case ' ':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
    case '\v':
        goto retry;
    // Identifier
    case '_':
    case 'a' ... 'z':
    case 'A' ... 'Z':
        for (;;) {
            switch (peek(self, 0)) {
            case '_':
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
                eat(self);
                break;
            default:
                goto end_ident;
            }
        }
end_ident:
        tk->kind = TK_IDENTIFIER;
        break;
    // Constant
    case '0':
        tk->val = 0;

        // Hex
        if (match(self, 'x') || match(self, 'X')) {
            for (;; eat(self)) {
                switch ((ch = peek(self, 0))) {
                case '0' ... '9':
                    tk->val = (tk->val << 4) | (ch - '0');
                    break;
                case 'a' ... 'f':
                    tk->val = (tk->val << 4) | (ch - 'a' + 0xa);
                    break;
                case 'A' ... 'F':
                    tk->val = (tk->val << 4) | (ch - 'A' + 0xa);
                    break;
                default:
                    goto end_integer;
                }
            }
        } else {
            // Octal
            for (;; eat(self)) {
                switch ((ch = peek(self, 0))) {
                case '0' ... '7':
                    tk->val = (tk->val << 3) | (ch - '0');
                    break;
                default:
                    goto end_integer;
                }
            }
        }
    case '1' ... '9':
        // Decimal
        tk->val = ch - '0';
        for (;; eat(self)) {
            switch ((ch = peek(self, 0))) {
            case '0' ... '9':
                tk->val = tk->val * 10 + ch - '0';
                break;
            default:
                goto end_integer;
            }
        }
end_integer:
        int_suffix(self);
        tk->kind = TK_CONSTANT;
        break;
    // Character constant
    case '\'':
        tk->val = 0;
        for (;;) {
            switch ((ch = input(self))) {
            case EOF:
            case '\n':
                tk->kind = TK_ERROR;
                goto end_char;
            case '\'':
                tk->kind = TK_CONSTANT;
                goto end_char;
            case '\\':
                ch = unescape(self);
                FALLTHROUGH;
            default:
                tk->val = (tk->val << 8) | ch;
            }
        }
end_char:
        break;
    // String literal
    case '\"':
        string_clear(&self->decode);
read_next_str:
        for (;;) {
            switch ((ch = input(self))) {
            case EOF:
            case '\n':
                tk->kind = TK_ERROR;
                break;
            case '\"':
                // Skip whitespace after the literal
                while (iswhite(peek(self, 0)))
                    eat(self);
                // Check if there is another one to concatenate it with
                if (match(self, '\"'))
                    goto read_next_str;
                tk->kind = TK_STR_LIT;
                goto end_str;
            case '\\':
                ch = unescape(self);
                FALLTHROUGH;
            default:
                string_push(&self->decode, ch);
            }
        }
end_str:
        tk->str = strtab_intern(self->decode.data)->key;
        break;
    case '[':
        tk->kind = TK_LSQ;
        break;
    case ']':
        tk->kind = TK_RSQ;
        break;
    case '(':
        tk->kind = TK_LPAREN;
        break;
    case ')':
        tk->kind = TK_RPAREN;
        break;
    case '{':
        tk->kind = TK_LCURLY;
        break;
    case '}':
        tk->kind = TK_RCURLY;
        break;
    case '~':
        tk->kind = TK_NOT;
        break;
    case '?':
        tk->kind = TK_COND;
        break;
    case ':':
        tk->kind = TK_COLON;
        break;
    case ';':
        tk->kind = TK_SEMICOLON;
        break;
    case ',':
        tk->kind = TK_COMMA;
        break;
    case '.':
        if (peek(self, 0) == '.' && peek(self, 1) == '.') {
            tk->kind = TK_ELLIPSIS;
            eat(self);
            eat(self);
        } else {
            tk->kind = TK_DOT;
        }
        break;
    case '!':
        if (match(self, '=')) {
            tk->kind = TK_NE;
        } else {
            tk->kind = TK_LNOT;
        }
        break;
    case '*':
        if (match(self, '=')) {
            tk->kind = TK_MUL_AS;
        } else {
            tk->kind = TK_MUL;
        }
        break;
    case '/':
        if (match(self, '*')) { // ANSI C comment
            do {
                while ((ch = input(self)) != '*' && ch != EOF) {
                    ;
                }
            } while (ch != EOF && !match(self, '/'));
            goto retry;
        }
        if (match(self, '/')) { // C++ comment
            while ((ch = input(self)) != '\n' && ch != EOF) {
                ;
            }
            goto retry;
        }
        if (match(self, '=')) {
            tk->kind = TK_DIV_AS;
        } else {
            tk->kind = TK_DIV;
        }
        break;
    case '%':
        if (match(self, '=')) {
            tk->kind = TK_MOD_AS;
        } else {
            tk->kind = TK_MOD;
        }
        break;
    case '+':
        if (match(self, '+')) {
            tk->kind = TK_INCR;
        } else if (match(self, '=')) {
            tk->kind = TK_ADD_AS;
        } else {
            tk->kind = TK_ADD;
        }
        break;
    case '-':
        if (match(self, '>')) {
            tk->kind = TK_ARROW;
        } else if (match(self, '-')) {
            tk->kind = TK_DECR;
        } else if (match(self, '=')) {
            tk->kind = TK_SUB_AS;
        } else {
            tk->kind = TK_SUB;
        }
        break;
    case '<':
        if (match(self, '<')) {
            if (match(self, '=')) {
                tk->kind = TK_LSH_AS;
            } else {
                tk->kind = TK_LSH;
            }
        } else if (match(self, '=')) {
            tk->kind = TK_LE;
        } else {
            tk->kind = TK_LT;
        }
        break;
    case '>':
        if (match(self, '>')) {
            if (match(self, '=')) {
                tk->kind = TK_RSH_AS;
            } else {
                tk->kind = TK_RSH;
            }
        } else if (match(self, '=')) {
            tk->kind = TK_GE;
        } else {
            tk->kind = TK_GT;
        }
        break;
    case '=':
        if (match(self, '=')) {
            tk->kind = TK_EQ;
        } else {
            tk->kind = TK_AS;
        }
        break;
    case '&':
        if (match(self, '&')) {
            tk->kind = TK_LAND;
        } else if (match(self, '=')) {
            tk->kind = TK_AND_AS;
        } else {
            tk->kind = TK_AND;
        }
        break;
    case '^':
        if (match(self, '=')) {
            tk->kind = TK_XOR_AS;
        } else {
            tk->kind = TK_XOR;
        }
        break;
    case '|':
        if (match(self, '|')) {
            tk->kind = TK_LOR;
        } else if (match(self, '=')) {
            tk->kind = TK_OR_AS;
        } else {
            tk->kind = TK_OR;
        }
        break;
    case '#':
        if (match(self, '#')) {
            tk->kind = TK_PASTE;
        } else {
            tk->kind = TK_HASH;
        }
        break;
    default:
        tk->kind = TK_ERROR;
    }

    int_entry_t *entry = strtab_intern(self->spelling.data);
    tk->spelling = entry->key;
    if (entry->value >= 0) {
        tk->kind = entry->value;
    }
}
