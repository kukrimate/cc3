#include "cc3.h"

const char *tk_str(int type)
{
    switch (type) {
    case TK_EOF:		return "EOF";

    case TK_AUTO:		return "auto";
    case TK_BREAK:		return "break";
    case TK_CASE:		return "case";
    case TK_CHAR:		return "char";
    case TK_CONST:		return "const";
    case TK_CONTINUE:	return "continue";
    case TK_DEFAULT:	return "default";
    case TK_DO:         return "do";
    case TK_DOUBLE:		return "double";
    case TK_ELSE:		return "else";
    case TK_ENUM:		return "enum";
    case TK_EXTERN:		return "extern";
    case TK_FLOAT:		return "float";
    case TK_FOR:		return "for";
    case TK_GOTO:		return "goto";
    case TK_IF:         return "if";
    case TK_INLINE:		return "inline";
    case TK_INT:		return "int";
    case TK_LONG:		return "long";
    case TK_REGISTER:   return "register";
    case TK_RESTRICT:   return "restrict";
    case TK_RETURN:		return "return";
    case TK_SHORT:		return "short";
    case TK_SIGNED:		return "signed";
    case TK_SIZEOF:		return "sizeof";
    case TK_STATIC:		return "static";
    case TK_STRUCT:		return "struct";
    case TK_SWITCH:		return "switch";
    case TK_TYPEDEF:    return "typedef";
    case TK_UNION:		return "union";
    case TK_UNSIGNED:   return "unsigned";
    case TK_VOID:		return "void";
    case TK_VOLATILE:   return "volatile";
    case TK_WHILE:		return "while";
    case TK_BOOL:		return "_Bool";
    case TK_COMPLEX:    return "_Complex";
    case TK_IMAGINARY:  return "_Imaginary";

    case TK_IDENTIFIER: return "IDENTIFIER";
    case TK_CONSTANT:   return "CONSTANT";
    case TK_STR_LIT:    return "STR_LIT";

    case TK_LSQ:		return "[";
    case TK_RSQ:		return "]";
    case TK_LPAREN:		return "(";
    case TK_RPAREN:		return ")";
    case TK_LCURLY:		return "{";
    case TK_RCURLY:		return "}";
    case TK_DOT:		return ".";
    case TK_ARROW:		return "->";
    case TK_INCR:		return "++";
    case TK_DECR:		return "--";
    case TK_AND:		return "&";
    case TK_MUL:		return "*";
    case TK_ADD:		return "+";
    case TK_SUB:		return "-";
    case TK_NOT:		return "~";
    case TK_LNOT:		return "!";
    case TK_DIV:		return "/";
    case TK_MOD:		return "%";
    case TK_LSH:		return "<<";
    case TK_RSH:		return ">>";
    case TK_LT:         return "<";
    case TK_GT:         return ">";
    case TK_LE:         return "<=";
    case TK_GE:         return ">=";
    case TK_EQ:         return "==";
    case TK_NE:         return "!=";
    case TK_XOR:		return "^";
    case TK_OR:         return "|";
    case TK_LAND:		return "&&";
    case TK_LOR:		return "||";
    case TK_COND:		return "?";
    case TK_COLON:		return ":";
    case TK_SEMICOLON:  return ";";
    case TK_ELLIPSIS:   return "...";
    case TK_AS:         return "=";
    case TK_MUL_AS:		return "*=";
    case TK_DIV_AS:		return "/=";
    case TK_MOD_AS:		return "%=";
    case TK_ADD_AS:		return "+=";
    case TK_SUB_AS:		return "-=";
    case TK_LSH_AS:		return "<<=";
    case TK_RSH_AS:		return ">>=";
    case TK_AND_AS:		return "&=";
    case TK_XOR_AS:		return "^=";
    case TK_OR_AS:		return "|=";
    case TK_COMMA:		return ",";
    case TK_HASH:		return "#";
    case TK_PASTE:		return "##";

    case TK_ERROR:      return "ERROR";

    default:            ASSERT_NOT_REACHED();
    }
}

void cc3_init(struct cc3 *self, FILE *fp)
{
    self->fp = fp;
    self->cur = self->buf;
    self->end = self->buf;

    self->tk_pos = 0;
    self->tk_cnt = 0;
}

static inline int lex_peek(struct cc3 *self, int i)
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

static inline int lex_input(struct cc3 *self)
{
    int ch = lex_peek(self, 0);

    // Increment current position if not at EOF
    if (ch != EOF)
        ++self->cur;

    // Return char we've just read
    return ch;
}

static inline _Bool lex_match(struct cc3 *self, int want)
{
    int ch = lex_peek(self, 0);

    // Increment position if it's a match
    if (ch == want) {
        ++self->cur;
        return 1;
    }

    return 0;
}

static int lex_next_internal(struct cc3 *self, struct tk_buf *tk)
{
    int ch;
    char *strptr = tk->strval;

retry:
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
        *strptr++ = ch;
        for (;;)
            switch ((ch = lex_peek(self, 0))) {
            case '_':
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
                *strptr++ = ch;
                ++self->cur;
                break;
            default:
                *strptr = 0;
                if (strcmp(tk->strval, "auto") == 0)        return TK_AUTO;
                if (strcmp(tk->strval, "break") == 0)       return TK_BREAK;
                if (strcmp(tk->strval, "case") == 0)        return TK_CASE;
                if (strcmp(tk->strval, "char") == 0)        return TK_CHAR;
                if (strcmp(tk->strval, "const") == 0)       return TK_CONST;
                if (strcmp(tk->strval, "continue") == 0)    return TK_CONTINUE;
                if (strcmp(tk->strval, "default") == 0)     return TK_DEFAULT;
                if (strcmp(tk->strval, "do") == 0)          return TK_DO;
                if (strcmp(tk->strval, "double") == 0)      return TK_DOUBLE;
                if (strcmp(tk->strval, "else") == 0)        return TK_ELSE;
                if (strcmp(tk->strval, "enum") == 0)        return TK_ENUM;
                if (strcmp(tk->strval, "extern") == 0)      return TK_EXTERN;
                if (strcmp(tk->strval, "float") == 0)       return TK_FLOAT;
                if (strcmp(tk->strval, "for") == 0)         return TK_FOR;
                if (strcmp(tk->strval, "goto") == 0)        return TK_GOTO;
                if (strcmp(tk->strval, "if") == 0)          return TK_IF;
                if (strcmp(tk->strval, "inline") == 0)      return TK_INLINE;
                if (strcmp(tk->strval, "int") == 0)         return TK_INT;
                if (strcmp(tk->strval, "long") == 0)        return TK_LONG;
                if (strcmp(tk->strval, "register") == 0)    return TK_REGISTER;
                if (strcmp(tk->strval, "restrict") == 0)    return TK_RESTRICT;
                if (strcmp(tk->strval, "return") == 0)      return TK_RETURN;
                if (strcmp(tk->strval, "short") == 0)       return TK_SHORT;
                if (strcmp(tk->strval, "signed") == 0)      return TK_SIGNED;
                if (strcmp(tk->strval, "sizeof") == 0)      return TK_SIZEOF;
                if (strcmp(tk->strval, "static") == 0)      return TK_STATIC;
                if (strcmp(tk->strval, "struct") == 0)      return TK_STRUCT;
                if (strcmp(tk->strval, "switch") == 0)      return TK_SWITCH;
                if (strcmp(tk->strval, "typedef") == 0)     return TK_TYPEDEF;
                if (strcmp(tk->strval, "union") == 0)       return TK_UNION;
                if (strcmp(tk->strval, "unsigned") == 0)    return TK_UNSIGNED;
                if (strcmp(tk->strval, "void") == 0)        return TK_VOID;
                if (strcmp(tk->strval, "volatile") == 0)    return TK_VOLATILE;
                if (strcmp(tk->strval, "while") == 0)       return TK_WHILE;
                if (strcmp(tk->strval, "_Bool") == 0)       return TK_BOOL;
                if (strcmp(tk->strval, "_Complex") == 0)    return TK_COMPLEX;
                if (strcmp(tk->strval, "_Imaginary") == 0)  return TK_IMAGINARY;
                return TK_IDENTIFIER;
            }
    // Constant
    case '0':
        tk->intval = 0;
        // Hex
        if ((ch = lex_peek(self, 0)) == 'x' || ch == 'X') {
            ++self->cur;
            for (;;)
                switch (lex_peek(self, 0)) {
                case '0' ... '9':
                    tk->intval = tk->intval * 10 + ch - '0';
                    ++self->cur;
                    break;
                case 'a' ... 'f':
                    tk->intval = tk->intval * 10 + ch - 'a' + 0xa;
                    ++self->cur;
                    break;
                case 'A' ... 'F':
                    tk->intval = tk->intval * 10 + ch - 'A' + 0xa;
                    ++self->cur;
                    break;
                default:
                    return TK_CONSTANT;
                }
        }
        // Octal
        for (;;)
            switch (lex_peek(self, 0)) {
            case '0' ... '7':
                tk->intval = tk->intval * 10 + ch - '0';
                ++self->cur;
                break;
            default:
                return TK_CONSTANT;
            }
    case '1' ... '9':
        // Decimal
        tk->intval = ch - '0';
        for (;;)
            switch ((ch = lex_peek(self, 0))) {
            case '0' ... '9':
                tk->intval = tk->intval * 10 + ch - '0';
                ++self->cur;
                break;
            default:
                return TK_CONSTANT;
            }
    // Character constant
    case '\'':
        for (;;)
            switch (lex_input(self)) {
            case EOF:
            case '\n':
                return TK_ERROR;
            case '\'':
                return TK_CONSTANT;
            case '\\':
                lex_input(self);
            }
    // String literal
    case '\"':
        for (;;)
            switch (lex_input(self)) {
            case EOF:
            case '\n':
                return TK_ERROR;
            case '\"':
                strcpy(strptr, "FIXME: string literal");
                return TK_STR_LIT;
            case '\\':
                lex_input(self);
            }
    case '[':   return TK_LSQ;
    case ']':   return TK_RSQ;
    case '(':   return TK_LPAREN;
    case ')':   return TK_RPAREN;
    case '{':   return TK_LCURLY;
    case '}':   return TK_RCURLY;
    case '~':   return TK_NOT;
    case '?':   return TK_COND;
    case ':':   return TK_COLON;
    case ';':   return TK_SEMICOLON;
    case ',':   return TK_COMMA;
    case '.':
        if (lex_peek(self, 0) == '.' && lex_peek(self, 1) == '.') {
            self->cur += 2;
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

static inline void lex_read_tok(struct cc3 *self)
{
    struct tk_buf *tk = self->tk_buf + (self->tk_pos + self->tk_cnt++) % LEX_TK_CNT;
    tk->type = lex_next_internal(self, tk);
}

struct tk_buf *lex_tok(struct cc3 *self, int i)
{
    // Make sure we don't look more than we are allowed
    assert(i < LEX_TK_CNT);

    // Add tokens until we have enough
    while (self->tk_cnt <= i)
        lex_read_tok(self);

    // Return pointer to i-th token
    return self->tk_buf + (self->tk_pos + i) % LEX_TK_CNT;
}

void lex_adv(struct cc3 *self)
{
    // Skip over the front token (if any)
    if (self->tk_cnt > 0) {
        self->tk_pos = (self->tk_pos + 1) % LEX_TK_CNT;
        --self->tk_cnt;
    }
}
