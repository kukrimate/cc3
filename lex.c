// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

void lex_init(lexer_t *self, int in_fd)
{
    self->in_fd = in_fd;

    self->cur = self->buf;
    self->end = self->buf;

    self->line = 1;
    self->col = 1;
}

void lex_free(lexer_t *self)
{
}

static int peek(lexer_t *self, int i)
{
    // Return character if in bounds
    if (self->cur + i < self->end)
        return self->cur[i];

    // Copy chars to the start of the buffer
    char *tmp = self->cur;
    self->cur = self->buf;
    while (tmp < self->end)
        *self->cur++ = *tmp++;

    // Fill buffer with characters
    self->end = self->cur + read(self->in_fd, self->cur, LEX_BUF_SZ / 2);

    // Return character if we read enough
    if (self->cur + i < self->end)
        return self->cur[i];

    // Otherwise we know this was EOF
    return EOF;
}

static int eat(lexer_t *self, tk_t *tk)
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
    string_push(&tk->spelling, ch);

    return ch;
}

static int input(lexer_t *self, tk_t *tk)
{
    int ch = peek(self, 0);
    // Increment current position if not at EOF
    if (ch != EOF)
        eat(self, tk);
    // Return char we've just read
    return ch;
}

static bool match(lexer_t *self, tk_t *tk, int want)
{
    int ch = peek(self, 0);
    // Increment position if it's a match
    if (ch == want) {
        eat(self, tk);
        return true;
    }
    return false;
}

static int check_keyword(const char *spelling)
{
    if (!strcmp(spelling, "auto"))          return TK_AUTO;
    if (!strcmp(spelling, "break"))         return TK_BREAK;
    if (!strcmp(spelling, "case"))          return TK_CASE;
    if (!strcmp(spelling, "char"))          return TK_CHAR;
    if (!strcmp(spelling, "const"))         return TK_CONST;
    if (!strcmp(spelling, "continue"))      return TK_CONTINUE;
    if (!strcmp(spelling, "default"))       return TK_DEFAULT;
    if (!strcmp(spelling, "do"))            return TK_DO;
    if (!strcmp(spelling, "double"))        return TK_DOUBLE;
    if (!strcmp(spelling, "else"))          return TK_ELSE;
    if (!strcmp(spelling, "enum"))          return TK_ENUM;
    if (!strcmp(spelling, "extern"))        return TK_EXTERN;
    if (!strcmp(spelling, "float"))         return TK_FLOAT;
    if (!strcmp(spelling, "for"))           return TK_FOR;
    if (!strcmp(spelling, "goto"))          return TK_GOTO;
    if (!strcmp(spelling, "if"))            return TK_IF;
    if (!strcmp(spelling, "inline"))        return TK_INLINE;
    if (!strcmp(spelling, "__inline"))      return TK_INLINE;   // [GNU]
    if (!strcmp(spelling, "int"))           return TK_INT;
    if (!strcmp(spelling, "long"))          return TK_LONG;
    if (!strcmp(spelling, "register"))      return TK_REGISTER;
    if (!strcmp(spelling, "restrict"))      return TK_RESTRICT;
    if (!strcmp(spelling, "return"))        return TK_RETURN;
    if (!strcmp(spelling, "short"))         return TK_SHORT;
    if (!strcmp(spelling, "signed"))        return TK_SIGNED;
    if (!strcmp(spelling, "sizeof"))        return TK_SIZEOF;
    if (!strcmp(spelling, "static"))        return TK_STATIC;
    if (!strcmp(spelling, "struct"))        return TK_STRUCT;
    if (!strcmp(spelling, "switch"))        return TK_SWITCH;
    if (!strcmp(spelling, "typedef"))       return TK_TYPEDEF;
    if (!strcmp(spelling, "union"))         return TK_UNION;
    if (!strcmp(spelling, "unsigned"))      return TK_UNSIGNED;
    if (!strcmp(spelling, "void"))          return TK_VOID;
    if (!strcmp(spelling, "volatile"))      return TK_VOLATILE;
    if (!strcmp(spelling, "while"))         return TK_WHILE;
    if (!strcmp(spelling, "_Bool"))         return TK_BOOL;
    if (!strcmp(spelling, "_Complex"))      return TK_COMPLEX;
    if (!strcmp(spelling, "_Imaginary"))    return TK_IMAGINARY;
    return TK_IDENTIFIER;
}

static int unescape(lexer_t *self, tk_t *tk)
{
    int val = input(self, tk);

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
        for (int ch;; eat(self, tk))
            switch ((ch = peek(self, 0))) {
            case '0' ... '7':
                val = val << 3 | (ch - '0');
                break;
            default:
                return val;
            }
    // Hex
    case 'x':
        for (int ch;; eat(self, tk))
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

static void int_suffix(lexer_t *self, tk_t *tk)
{
    // FIXME: this is not really correct
    while (match(self, tk, 'u') || match(self, tk, 'U')
            || match(self, tk, 'l') || match(self, tk, 'L'))
        ;
}

int lex_next(lexer_t *self, tk_t *tk)
{
    int ch;

retry:
    // Clear token spelling
    string_clear(&tk->spelling);

    // Save token starting position
    tk->line = self->line;
    tk->col = self->col;

    switch ((ch = input(self, tk))) {
    // End of file
    case EOF:
        return (tk->type = TK_EOF);
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
        for (;; eat(self, tk))
            switch (peek(self, 0)) {
            case '_':
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
                break;
            default:
                return (tk->type = check_keyword(tk->spelling.data));
            }
    // Constant
    case '0':
        // Hex
        if (match(self, tk, 'x') || match(self, tk, 'X')) {
            tk->val = 0;
            for (;; eat(self, tk))
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
                    int_suffix(self, tk);
                    return (tk->type = TK_CONSTANT);
                }
        }
        // Octal
        tk->val = 0;
        for (;; eat(self, tk))
            switch ((ch = peek(self, 0))) {
            case '0' ... '7':
                tk->val = (tk->val << 3) | (ch - '0');
                break;
            default:
                int_suffix(self, tk);
                return (tk->type = TK_CONSTANT);
            }
    case '1' ... '9':
        // Decimal
        tk->val = ch - '0';
        for (;; eat(self, tk))
            switch ((ch = peek(self, 0))) {
            case '0' ... '9':
                tk->val = tk->val * 10 + ch - '0';
                break;
            default:
                int_suffix(self, tk);
                return (tk->type = TK_CONSTANT);
            }
    // Character constant
    case '\'':
        tk->val = 0;
        for (;;)
            switch ((ch = input(self, tk))) {
            case EOF:
            case '\n':
                return (tk->type = TK_ERROR);
            case '\'':
                return (tk->type = TK_CONSTANT);
            case '\\':
                ch = unescape(self, tk);
                FALLTHROUGH;
            default:
                tk->val = (tk->val << 8) | ch;
            }
    // String literal
    case '\"':
        string_clear(&tk->str);
        for (;;)
            switch ((ch = input(self, tk))) {
            case EOF:
            case '\n':
                return (tk->type = TK_ERROR);
            case '\"':
                return (tk->type = TK_STR_LIT);
            case '\\':
                ch = unescape(self, tk);
                FALLTHROUGH;
            default:
                string_push(&tk->str, ch);
            }
    case '[':
        return (tk->type = TK_LSQ);
    case ']':
        return (tk->type = TK_RSQ);
    case '(':
        return (tk->type = TK_LPAREN);
    case ')':
        return (tk->type = TK_RPAREN);
    case '{':
        return (tk->type = TK_LCURLY);
    case '}':
        return (tk->type = TK_RCURLY);
    case '~':
        return (tk->type = TK_NOT);
    case '?':
        return (tk->type = TK_COND);
    case ':':
        return (tk->type = TK_COLON);
    case ';':
        return (tk->type = TK_SEMICOLON);
    case ',':
        return (tk->type = TK_COMMA);
    case '.':
        if (peek(self, 0) == '.' && peek(self, 1) == '.') {
            eat(self, tk);
            eat(self, tk);
            return (tk->type = TK_ELLIPSIS);
        }
        return (tk->type = TK_DOT);
    case '!':
        if (match(self, tk, '='))
            return (tk->type = TK_NE);
        return (tk->type = TK_LNOT);
    case '*':
        if (match(self, tk, '='))
            return (tk->type = TK_MUL_AS);
        return (tk->type = TK_MUL);
    case '/':
        // ANSI C comment
        if (match(self, tk, '*')) {
            do
                while ((ch = input(self, tk)) != '*' && ch != EOF);
            while (ch != EOF && !match(self, tk, '/'));
            goto retry;
        }
        // C++ comment
        if (match(self, tk, '/')) {
            while ((ch = input(self, tk)) != '\n' && ch != EOF);
            goto retry;
        }
        if (match(self, tk, '='))
            return (tk->type = TK_DIV_AS);
        return (tk->type = TK_DIV);
    case '%':
        if (match(self, tk, '='))
            return (tk->type = TK_MOD_AS);
        return (tk->type = TK_MOD);
    case '+':
        if (match(self, tk, '+'))
            return (tk->type = TK_INCR);
        if (match(self, tk, '='))
            return (tk->type = TK_ADD_AS);
        return (tk->type = TK_ADD);
    case '-':
        if (match(self, tk, '>'))
            return (tk->type = TK_ARROW);
        if (match(self, tk, '-'))
            return (tk->type = TK_DECR);
        if (match(self, tk, '='))
            return (tk->type = TK_SUB_AS);
        return (tk->type = TK_SUB);
    case '<':
        if (match(self, tk, '<')) {
            if (match(self, tk, '='))
                return (tk->type = TK_LSH_AS);
            return (tk->type = TK_LSH);
        }
        if (match(self, tk, '='))
            return (tk->type = TK_LE);
        return (tk->type = TK_LT);
    case '>':
        if (match(self, tk, '>')) {
            if (match(self, tk, '='))
                return (tk->type = TK_RSH_AS);
            return (tk->type = TK_RSH);
        }
        if (match(self, tk, '='))
            return (tk->type = TK_GE);
        return (tk->type = TK_GT);
    case '=':
        if (match(self, tk, '='))
            return (tk->type = TK_EQ);
        return (tk->type = TK_AS);
    case '&':
        if (match(self, tk, '&'))
            return (tk->type = TK_LAND);
        if (match(self, tk, '='))
            return (tk->type = TK_AND_AS);
        return (tk->type = TK_AND);
    case '^':
        if (match(self, tk, '='))
            return (tk->type = TK_XOR_AS);
        return (tk->type = TK_XOR);
    case '|':
        if (match(self, tk, '|'))
            return (tk->type = TK_LOR);
        if (match(self, tk, '='))
            return (tk->type = TK_OR_AS);
        return (tk->type = TK_OR);
    case '#':
        if (match(self, tk, '#'))
            return (tk->type = TK_PASTE);
        return (tk->type = TK_HASH);
    // Unknown character
    default:
        return (tk->type = TK_ERROR);
    }
}
