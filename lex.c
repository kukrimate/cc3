#include "cc3.h"

void lex_init(lexer_t *self, FILE *fp)
{
    // Setup input buffer
    self->fp = fp;
    self->cur = self->buf;
    self->end = self->buf;

    // Setup token buffer
    self->tk_pos = 0;
    self->tk_cnt = 0;

    for (int i = 0; i < LEX_TK_CNT; ++i) {
        string_init(&self->tk_buf[i].spelling);
        string_init(&self->tk_buf[i].str);
    }
}

void lex_free(lexer_t *self)
{
    for (int i = 0; i < LEX_TK_CNT; ++i) {
        string_free(&self->tk_buf[i].spelling);
        string_init(&self->tk_buf[i].str);
    }
}

static int lex_peek(lexer_t *self, int i)
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
    self->end = self->cur + fread(self->cur, 1, LEX_BUF_SZ / 2, self->fp);

    // Return character if we read enough
    if (self->cur + i < self->end)
        return self->cur[i];

    // Otherwise we know this was EOF
    return EOF;
}

static int lex_eat(lexer_t *self)
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
    string_push(&self->tk->spelling, ch);

    return ch;
}

static int lex_input(lexer_t *self)
{
    int ch = lex_peek(self, 0);
    // Increment current position if not at EOF
    if (ch != EOF)
        lex_eat(self);
    // Return char we've just read
    return ch;
}

static bool lex_match(lexer_t *self, int want)
{
    int ch = lex_peek(self, 0);
    // Increment position if it's a match
    if (ch == want) {
        lex_eat(self);
        return true;
    }
    return false;
}

static int lex_check_keyword(lexer_t *self)
{
    const char *spelling = self->tk->spelling.data;

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
    if (!strcmp(spelling, "int"))           return TK_INT;
    if (!strcmp(spelling, "long"))          return TK_LONG;
    if (!strcmp(spelling, "register"))      return TK_REGISTER;
    if (!strcmp(spelling, "restrict"))      return TK_RESTRICT;
    if (!strcmp(spelling, "return "))       return TK_RETURN;
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

static int lex_unescape(lexer_t *self)
{
    int val = lex_input(self);

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
        for (int ch;; lex_adv(self))
            switch ((ch = lex_peek(self, 0))) {
            case '0' ... '7':
                val = val << 3 | (ch - '0');
                break;
            default:
                return val;
            }
    // Hex
    case 'x':
        for (int ch;; lex_adv(self))
            switch ((ch = lex_peek(self, 0))) {
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

static int lex_next(lexer_t *self)
{
    int ch;

retry:
    // Clear token spelling
    string_clear(&self->tk->spelling);

    // Save token starting position
    self->tk->line = self->line;
    self->tk->col = self->col;

    switch ((ch = lex_input(self))) {
    // End of file
    case EOF:
        return TK_EOF;
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
        string_clear(&self->tk->str);
        string_push(&self->tk->str, ch);
        for (;; lex_eat(self))
            switch ((ch = lex_peek(self, 0))) {
            case '_':
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
                string_push(&self->tk->str, ch);
                break;
            default:
                return lex_check_keyword(self);
            }
    // Constant
    case '0':
        // Hex
        if (lex_match(self, 'x') || lex_match(self, 'X')) {
            self->tk->val = 0;
            for (;; lex_eat(self))
                switch ((ch = lex_peek(self, 0))) {
                case '0' ... '9':
                    self->tk->val = (self->tk->val << 4) | (ch - '0');
                    break;
                case 'a' ... 'f':
                    self->tk->val = (self->tk->val << 4) | (ch - 'a' + 0xa);
                    break;
                case 'A' ... 'F':
                    self->tk->val = (self->tk->val << 4) | (ch - 'A' + 0xa);
                    break;
                default:
                    return TK_CONSTANT;
                }
        }
        // Octal
        self->tk->val = 0;
        for (;; lex_eat(self))
            switch (lex_peek(self, 0)) {
            case '0' ... '7':
                self->tk->val = (self->tk->val << 3) | (ch - '0');
                break;
            default:
                return TK_CONSTANT;
            }
    case '1' ... '9':
        // Decimal
        self->tk->val = ch - '0';
        for (;; lex_eat(self))
            switch (lex_peek(self, 0)) {
            case '0' ... '9':
                self->tk->val = self->tk->val * 10 + ch - '0';
                break;
            default:
                return TK_CONSTANT;
            }
    // Character constant
    case '\'':
        self->tk->val = 0;
        for (;;)
            switch ((ch = lex_input(self))) {
            case EOF:
            case '\n':
                return TK_ERROR;
            case '\'':
                return TK_CONSTANT;
            case '\\':
                ch = lex_unescape(self);
                // FALLTHROUGH
            default:
                self->tk->val = (self->tk->val << 8) | ch;
            }
    // String literal
    case '\"':
        string_clear(&self->tk->str);
        for (;;)
            switch ((ch = lex_input(self))) {
            case EOF:
            case '\n':
                return TK_ERROR;
            case '\"':
                return TK_STR_LIT;
            case '\\':
                ch = lex_unescape(self);
                // FALLTHROUGH
            default:
                string_push(&self->tk->str, ch);
            }
    case '[':
        return TK_LSQ;
    case ']':
        return TK_RSQ;
    case '(':
        return TK_LPAREN;
    case ')':
        return TK_RPAREN;
    case '{':
        return TK_LCURLY;
    case '}':
        return TK_RCURLY;
    case '~':
        return TK_NOT;
    case '?':
        return TK_COND;
    case ':':
        return TK_COLON;
    case ';':
        return TK_SEMICOLON;
    case ',':
        return TK_COMMA;
    case '.':
        if (lex_peek(self, 0) == '.' && lex_peek(self, 1) == '.') {
            lex_eat(self);
            lex_eat(self);
            return TK_ELLIPSIS;
        }
        return TK_DOT;
    case '!':
        if (lex_match(self, '='))
            return TK_NE;
        return TK_LNOT;
    case '*':
        if (lex_match(self, '='))
            return TK_MUL_AS;
        return TK_MUL;
    case '/':
        // ANSI C comment
        if (lex_match(self, '*')) {
            do
                while ((ch = lex_input(self)) != '*' && ch != EOF);
            while ((ch = lex_input(self)) != '/' && ch != EOF);
            goto retry;
        }
        // C++ comment
        if (lex_match(self, '/')) {
            while ((ch = lex_input(self)) != '\n' && ch != EOF);
            goto retry;
        }
        if (lex_match(self, '='))
            return TK_DIV_AS;
        return TK_DIV;
    case '%':
        if (lex_match(self, '='))
            return TK_MOD_AS;
        return TK_MOD;
    case '+':
        if (lex_match(self, '+'))
            return TK_INCR;
        if (lex_match(self, '='))
            return TK_ADD_AS;
        return TK_ADD;
    case '-':
        if (lex_match(self, '>'))
            return TK_ARROW;
        if (lex_match(self, '-'))
            return TK_DECR;
        if (lex_match(self, '='))
            return TK_SUB_AS;
        return TK_SUB;
    case '<':
        if (lex_match(self, '<')) {
            if (lex_match(self, '='))
                return TK_LSH_AS;
            return TK_LSH;
        }
        if (lex_match(self, '='))
            return TK_LE;
        return TK_LT;
    case '>':
        if (lex_match(self, '>')) {
            if (lex_match(self, '='))
                return TK_RSH_AS;
            return TK_RSH;
        }
        if (lex_match(self, '='))
            return TK_GE;
        return TK_GT;
    case '=':
        if (lex_match(self, '='))
            return TK_EQ;
        return TK_AS;
    case '&':
        if (lex_match(self, '&'))
            return TK_LAND;
        if (lex_match(self, '='))
            return TK_AND_AS;
        return TK_AND;
    case '^':
        if (lex_match(self, '='))
            return TK_XOR_AS;
        return TK_XOR;
    case '|':
        if (lex_match(self, '|'))
            return TK_LOR;
        if (lex_match(self, '='))
            return TK_OR_AS;
        return TK_OR;
    case '#':
        if (lex_match(self, '#'))
            return TK_PASTE;
        return TK_HASH;
    // Unknown character
    default:
        return TK_ERROR;
    }
}

tk_t *lex_tok(lexer_t *self, int i)
{
    // Make sure lookahead doesn't go too far
    assert(i < LEX_TK_CNT);

    // Add tokens until we have enough
    while (self->tk_cnt <= i) {
        self->tk = self->tk_buf + (self->tk_pos + self->tk_cnt++) % LEX_TK_CNT;
        self->tk->type = lex_next(self);
    }

    // Return pointer to i-th token
    return self->tk_buf + (self->tk_pos + i) % LEX_TK_CNT;
}

void lex_adv(lexer_t *self)
{
    // Make sure the buffer is not empty
    assert(self->tk_cnt > 0);

    // Skip over the front token
    self->tk_pos = (self->tk_pos + 1) % LEX_TK_CNT;
    --self->tk_cnt;
}
