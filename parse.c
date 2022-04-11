#include "cc3.h"

static void err(const char *fmt, ...)
{
    // Print prefix
    fputs("Parse error: ", stderr);

    // Format error
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    // Print newline
    fputc('\n', stderr);

    // Panic
    abort();
}

static void want(struct cc3 *self, int type)
{
    struct tk_buf *tk = lex_tok(self, 0);
    if (tk->type != type)
        err("Unexpected token %s expected %s", tk_str(tk->type), tk_str(type));
    lex_adv(self);
}

static struct tk_buf *maybe_want(struct cc3 *self, int type)
{
    struct tk_buf *tk = lex_tok(self, 0);
    if (tk->type == type) {
        lex_adv(self);
        return tk;
    }
    return NULL;
}

/** Expressions **/

static void primary_expression(struct cc3 *self);
static void postfix_expression(struct cc3 *self);
static void unary_expression(struct cc3 *self);
static void cast_expression(struct cc3 *self);
static void multiplicative_expression(struct cc3 *self);
static void additive_expression(struct cc3 *self);
static void shift_expression(struct cc3 *self);
static void relational_expression(struct cc3 *self);
static void equality_expression(struct cc3 *self);
static void and_expression(struct cc3 *self);
static void xor_expression(struct cc3 *self);
static void or_expression(struct cc3 *self);
static void land_expression(struct cc3 *self);
static void lor_expression(struct cc3 *self);
static void conditional_expression(struct cc3 *self);
static void assignment_expression(struct cc3 *self);
static void expression(struct cc3 *self);
static void constant_expression(struct cc3 *self);

void primary_expression(struct cc3 *self)
{
    struct tk_buf *tk = lex_tok(self, 0);

    switch (tk->type) {
    case TK_IDENTIFIER:
    case TK_CONSTANT:
    case TK_STR_LIT:
        lex_adv(self);
        break;
    case TK_LPAREN:
        lex_adv(self);
        expression(self);
        want(self, TK_RPAREN);
        break;
    default:
        err("Invalid primary expression %s", tk_str(tk->type));
    }
}

void postfix_expression(struct cc3 *self)
{
    primary_expression(self);

    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            expression(self);
            want(self, TK_RSQ);
            continue;
        }

        if (maybe_want(self, TK_LPAREN)) {
            // Empty argument list
            if (maybe_want(self, TK_RPAREN))
                continue;

            // Otherwise we have a comma seperated list of assignment-expressions
            do
                assignment_expression(self);
            while (maybe_want(self, TK_COMMA));

            want(self, TK_RPAREN);
            continue;
        }

        if (maybe_want(self, TK_DOT)) {
            want(self, TK_IDENTIFIER);
            continue;
        }

        if (maybe_want(self, TK_ARROW)) {
            want(self, TK_IDENTIFIER);
            continue;
        }

        if (maybe_want(self, TK_INCR))
            ;

        if (maybe_want(self, TK_DECR))
            ;

        break;
    }

    // FIXME: recognize compound literals here
}

void unary_expression(struct cc3 *self)
{
    if (maybe_want(self, TK_INCR))
        unary_expression(self);
    else if (maybe_want(self, TK_DECR))
        unary_expression(self);
    else if (maybe_want(self, TK_AND))
        cast_expression(self);
    else if (maybe_want(self, TK_MUL))
        cast_expression(self);
    else if (maybe_want(self, TK_ADD))
        cast_expression(self);
    else if (maybe_want(self, TK_SUB))
        cast_expression(self);
    else if (maybe_want(self, TK_NOT))
        cast_expression(self);
    else if (maybe_want(self, TK_LNOT))
        cast_expression(self);
    else
        postfix_expression(self);

    // FIXME: sizeof unary-expression
    //    and sizeof '(' type-name ')' here
}

void cast_expression(struct cc3 *self)
{
    unary_expression(self);
    // FIXME: recognize '(' type-name ')' here
}

void multiplicative_expression(struct cc3 *self)
{
    cast_expression(self);
    for (;;)
        if (maybe_want(self, TK_MUL))
            cast_expression(self);
        else if (maybe_want(self, TK_DIV))
            cast_expression(self);
        else if (maybe_want(self, TK_MOD))
            cast_expression(self);
        else
            return;
}

void additive_expression(struct cc3 *self)
{
    multiplicative_expression(self);
    for (;;)
        if (maybe_want(self, TK_ADD))
            multiplicative_expression(self);
        else if (maybe_want(self, TK_SUB))
            multiplicative_expression(self);
        else
            return;
}

void shift_expression(struct cc3 *self)
{
    additive_expression(self);
    for (;;)
        if (maybe_want(self, TK_LSH))
            additive_expression(self);
        else if (maybe_want(self, TK_RSH))
            additive_expression(self);
        else
            return;
}

void relational_expression(struct cc3 *self)
{
    shift_expression(self);
    for (;;)
        if (maybe_want(self, TK_LT))
            shift_expression(self);
        else if (maybe_want(self, TK_GT))
            shift_expression(self);
        else if (maybe_want(self, TK_LE))
            shift_expression(self);
        else if (maybe_want(self, TK_GE))
            shift_expression(self);
        else
            return;
}

void equality_expression(struct cc3 *self)
{
    relational_expression(self);
    for (;;)
        if (maybe_want(self, TK_EQ))
            relational_expression(self);
        else if (maybe_want(self, TK_NE))
            relational_expression(self);
        else
            return;
}

void and_expression(struct cc3 *self)
{
    equality_expression(self);
    while (maybe_want(self, TK_AND))
        equality_expression(self);
}

void xor_expression(struct cc3 *self)
{
    and_expression(self);
    while (maybe_want(self, TK_XOR))
        and_expression(self);
}

void or_expression(struct cc3 *self)
{
    xor_expression(self);
    while (maybe_want(self, TK_OR))
        xor_expression(self);
}

void land_expression(struct cc3 *self)
{
    or_expression(self);
    while (maybe_want(self, TK_LAND))
        or_expression(self);
}

void lor_expression(struct cc3 *self)
{
    land_expression(self);
    while (maybe_want(self, TK_LOR))
        land_expression(self);
}

void conditional_expression(struct cc3 *self)
{
    lor_expression(self);

    if (!maybe_want(self, TK_COND))
        return;

    expression(self);

    want(self, TK_COLON);

    conditional_expression(self);
}

void assignment_expression(struct cc3 *self)
{
    conditional_expression(self);

    if (maybe_want(self, TK_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_MUL_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_DIV_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_MOD_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_ADD_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_SUB_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_LSH_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_RSH_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_AND_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_XOR_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_OR_AS))
        assignment_expression(self);
}

void expression(struct cc3 *self)
{
    assignment_expression(self);
    while (maybe_want(self, TK_COMMA))
        assignment_expression(self);
}

void constant_expression(struct cc3 *self)
{
    conditional_expression(self);
}

/** Declarations **/

static bool declaration(struct cc3 *self);
static bool declaration_specifiers(struct cc3 *self);
static void struct_declaration_list(struct cc3 *self);
static void enumerator_list(struct cc3 *self);
static void declarator(struct cc3 *self, bool allow_abstract);
static void type_qualifier_list(struct cc3 *self);
static void initializer(struct cc3 *self);
static void initializer_list(struct cc3 *self);
static void designation(struct cc3 *self);

bool declaration(struct cc3 *self)
{
    if (!declaration_specifiers(self))
        return false;

    if (!maybe_want(self, TK_SEMICOLON)) {
        // Read declarators and initializers
        do {
            declarator(self, false);
            if (maybe_want(self, TK_AS))
                initializer(self);
        } while (maybe_want(self, TK_COMMA));

        // Declarators must end with ;
        want(self, TK_SEMICOLON);
    }

    return true;
}

bool declaration_specifiers(struct cc3 *self)
{
    bool match = false;
    int sc = -1;

    for (;;) {
        // Read token
        struct tk_buf *tk = lex_tok(self, 0);

        // Match token
        switch (tk->type) {

        // Storage class
        case TK_TYPEDEF:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_AUTO:
        case TK_REGISTER:

            if (sc < 0)
                sc = tk->type;
            else
                err("Junk storage class %s already had %s", tk_str(tk->type), tk_str(sc));

            break;

        // Type specifier
        case TK_VOID:
        case TK_CHAR:
        case TK_SHORT:
        case TK_INT:
        case TK_LONG:
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_SIGNED:
        case TK_UNSIGNED:
        case TK_BOOL:
        case TK_COMPLEX:
        case TK_IMAGINARY:
            break;

        case TK_STRUCT:
        case TK_UNION:
            lex_adv(self);

            // Might be followed by an identifier
            if ((tk = maybe_want(self, TK_IDENTIFIER)))
                ;

            // Read member declarations if present
            if (maybe_want(self, TK_LCURLY))
                struct_declaration_list(self);

            continue;

        case TK_ENUM:
            lex_adv(self);

            // Might be followed by an identifier
            if ((tk = maybe_want(self, TK_IDENTIFIER)))
                ;

            // Read enumerators if present
            if (maybe_want(self, TK_LCURLY))
                enumerator_list(self);

            continue;

        // Type qualifier
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            break;

        // Function specifier
        case TK_INLINE:
            break;

        // End of declaration specifiers
        default:
            return match;
        }

        // Consume token
        lex_adv(self);
        // Note that we've matched something
        match = true;
    }
}

void struct_declaration_list(struct cc3 *self)
{
    do {
        if (!declaration_specifiers(self))
            err("Expected declaration in struct");

        do {
            if (maybe_want(self, TK_COLON)) {   // Anonymous bitfield
                constant_expression(self);
                continue;
            }

            declarator(self, false);
            if (maybe_want(self, TK_COLON))
                constant_expression(self);

        } while (!maybe_want(self, TK_SEMICOLON));

    } while (!maybe_want(self, TK_RCURLY));
}

void enumerator_list(struct cc3 *self)
{
    for (;;) {
        // Read enumerator name
        want(self, TK_IDENTIFIER);

        // Optionally there might be a value
        if (maybe_want(self, TK_AS))
            constant_expression(self);

        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))    // Trailing comma allowed
                return;
        } else {
            want(self, TK_RCURLY);              // Otherwise the list must end
            return;
        }
    }
}

void declarator(struct cc3 *self, bool allow_abstract)
{
    // Read all pointer prefixes (including their qualifiers)
    while (maybe_want(self, TK_MUL))
        type_qualifier_list(self);

    if (maybe_want(self, TK_LPAREN)) {  // '(' declarator ')'
        // FIXME: in function declarators, only recurse if the follow set is
        // *not* typedef-name )
        declarator(self, allow_abstract);
        want(self, TK_RPAREN);
    } else {
        if (allow_abstract)
            maybe_want(self, TK_IDENTIFIER);
        else
            want(self, TK_IDENTIFIER);
    }

    // Read all array or function suffixes
    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            // Function argument declared as arrays (really pointers) can be
            // qualified like normal pointers
            // Additionally static can be provided as an optimization hint,
            // we can just ignore this for now
            type_qualifier_list(self);
            maybe_want(self, TK_STATIC);
            type_qualifier_list(self);

            // In our implementation we only allow constant array lengths, as
            // VLA support is left out
            if (!maybe_want(self, TK_RSQ)) {
                constant_expression(self);
                want(self, TK_RSQ);
            }

            continue;
        }

        if (maybe_want(self, TK_LPAREN)) {
            // An empty list is allowed, it means "unspecified number of parameters"
            if (maybe_want(self, TK_RPAREN))
                continue;

            // If there is something it's either a K&R identifier list
            if (maybe_want(self, TK_IDENTIFIER)) {
                // Read identifiers until no comma was found
                while (maybe_want(self, TK_COMMA))
                    want(self, TK_IDENTIFIER);

                // ) must follow the identifier list
                want(self, TK_RPAREN);

                continue;
            }

            // Or otherwise we have an ANSI C parameter type list
            while (declaration_specifiers(self)) {
                // Parameter declarator must follow
                // (this can also be an abstract declarator)
                declarator(self, true);

                if (maybe_want(self, TK_COMMA)) {
                    // If there is a comma, ellipsis might follow
                    if (maybe_want(self, TK_ELLIPSIS))
                        break;
                    // Otherwise if there is no ellipsis we just continue
                } else {
                    // No comma means we're at the end for sure
                    break;
                }
            }

            want(self, TK_RPAREN);
            continue;
        }

        // Nothing matched, thus we are done with suffixes
        break;
    }
}

void type_qualifier_list(struct cc3 *self)
{
    for (;;)
        switch (lex_tok(self, 0)->type) {
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            lex_adv(self);
            break;
        default:
            return;
        }
}

void initializer(struct cc3 *self)
{
    if (maybe_want(self, TK_LCURLY))
        initializer_list(self);
    else
        assignment_expression(self);
}

void initializer_list(struct cc3 *self)
{
    for (;;) {
        designation(self);
        initializer(self);

        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))    // Trailing comma allowed
                return;
        } else {
            want(self, TK_RCURLY);              // Otherwise the list must end
            return;
        }
    }
}

void designation(struct cc3 *self)
{
    bool match = false;

    for (;;)
        if (maybe_want(self, TK_LSQ)) {
            constant_expression(self);
            want(self, TK_RSQ);
            match = true;
        } else if (maybe_want(self, TK_DOT)) {
            want(self, TK_IDENTIFIER);
            match = true;
        } else {
            goto end;
        }
end:
    if (match)
        want(self, TK_AS);
}


/** Statements **/

static void statement(struct cc3 *self);
static void block_item_list(struct cc3 *self);

void statement(struct cc3 *self)
{
    // Statements may be preceeded by labels

    if (lex_tok(self, 0)->type == TK_IDENTIFIER
            && lex_tok(self, 1)->type == TK_COLON) {    // IDENTIFIER ':'
        lex_adv(self);
        lex_adv(self);
    } else if (maybe_want(self, TK_CASE)) {             // CASE
        constant_expression(self);
        want(self, TK_COLON);
    } else if (maybe_want(self, TK_DEFAULT)) {          // DEFAULT
        want(self, TK_COLON);
    }

    // Compound statement
    if (maybe_want(self, TK_LCURLY)) {
        block_item_list(self);
        return;
    }

    // If statement
    if (maybe_want(self, TK_IF)) {
        // Heading
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);

        // Then
        statement(self);

        // Else
        if (maybe_want(self, TK_ELSE))
            statement(self);

        return;
    }

    // Switch statement
    if (maybe_want(self, TK_SWITCH)) {
        // Heading
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);

        // Body
        statement(self);

        return;
    }

    // While statement
    if (maybe_want(self, TK_WHILE)) {
        // Heading
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);

        // Body
        statement(self);

        return;
    }

    // Do ... while statement
    if (maybe_want(self, TK_DO)) {
        // Body
        statement(self);
        // Condition
        want(self, TK_WHILE);
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);
        want(self, TK_SEMICOLON);

        return;
    }

    // For statement
    if (maybe_want(self, TK_FOR)) {
        // Heading
        want(self, TK_LPAREN);

        if (!declaration(self))
            if (!maybe_want(self, TK_SEMICOLON)) {
                expression(self);
                want(self, TK_SEMICOLON);
            }

        if (!maybe_want(self, TK_SEMICOLON)) {
            expression(self);
            want(self, TK_SEMICOLON);
        }
        if (!maybe_want(self, TK_RPAREN)) {
            expression(self);
            want(self, TK_RPAREN);
        }

        // Body
        statement(self);

        return;
    }

    // Jumps
    if (maybe_want(self, TK_GOTO)) {
        want(self, TK_IDENTIFIER);
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_CONTINUE)) {
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_BREAK)) {
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_RETURN)) {
        if (!maybe_want(self, TK_SEMICOLON)) {
            expression(self);
            want(self, TK_SEMICOLON);
        }
        return;
    }

    // Nothing else left, it must be an expression statement
    if (!maybe_want(self, TK_SEMICOLON)) {
        expression(self);
        want(self, TK_SEMICOLON);
    }
}

void block_item_list(struct cc3 *self)
{
    while (!maybe_want(self, TK_RCURLY))
        if (!declaration(self))
            statement(self);
}

void parse(struct cc3 *self)
{
    while (declaration_specifiers(self)) {
        // No declarators
        if (maybe_want(self, TK_SEMICOLON))
            continue;

        // Read first declarator
        declarator(self, false);

        // Check for function
        if (maybe_want(self, TK_LCURLY)) {
            block_item_list(self);
        } else {
            // Otherwise there might be more declarators
            while (maybe_want(self, TK_COMMA))
                declarator(self, false);

            // And the list must end with a ;
            want(self, TK_SEMICOLON);
        }
    }
}
