// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

VEC_DEF(tk_vec, tk_t)
VEC_GEN(tk_vec, tk_t)

//
// Stored macro definition
//

typedef struct {
    // Name of macro
    char *name;
    // Is it a function like macro?
    bool is_function;
    // Is it a variadic macro?
    bool is_variadic;
    // Number of parameters
    size_t param_cnt;
    // Replacement list of the macro
    tk_vec_t replacement_list;
} macro_t;

VEC_DEF(macro_vec, macro_t)
VEC_GEN(macro_vec, macro_t)

//
// Macro expansion frames
//

typedef struct {
    size_t index;
    tk_vec_t tokens;
} exp_t;

VEC_DEF(exp_vec, exp_t)
VEC_GEN(exp_vec, exp_t)

static void enter_expansion(exp_vec_t *stack, tk_vec_t *tokens)
{
    exp_t *exp = exp_vec_push(stack);
    exp->index = 0;
    exp->tokens = *tokens;
}

static tk_t *next_in_expansion(exp_vec_t *stack)
{
    // While there are frame
    while (stack->length > 0) {
        // Return a token from the active frame
        exp_t *exp = VEC_BACK(stack);
        if (exp->index < exp->tokens.length)
            return VEC_AT(&exp->tokens, exp->index++);
        // Or otherwise pop the frame and continue
        VEC_POP(stack);
    }
    // If there are no frames with tokens return NULL
    return NULL;
}

typedef struct {
    // Name of the file
    char *name;
    // Corresponding lexical analyzer
    lexer_t lexer;
    // Currently lexed token
    tk_t lexed_tk;
    // Macro expansion stack
    exp_vec_t stack;
    // Unconsumed previous token
    tk_t *peek_tk;
} file_t;

static void file_init(file_t *file, char *name, int in_fd)
{
    // Save filename and setup lexer
    file->name = name;
    lex_init(&file->lexer, in_fd);
    // Initialize lexed token
    string_init(&file->lexed_tk.spelling);
    string_init(&file->lexed_tk.str);
    // Initialize expansion stack
    exp_vec_init(&file->stack);
    // There was no unconsumed previous token
    file->peek_tk = NULL;
}

static tk_t *peek(file_t *file)
{
    // First return existing token if any
    if (file->peek_tk)
        return file->peek_tk;

    // Then try reading from macro expansion
    if ((file->peek_tk = next_in_expansion(&file->stack)))
        return file->peek_tk;

    // Finally lex a new token
    lex_next(&file->lexer, &file->lexed_tk);
    return (file->peek_tk = &file->lexed_tk);
}

static void adv(file_t *file)
{
    assert(file->peek_tk != NULL);
    file->peek_tk = NULL;
}

static tk_t *next_on_line(file_t *file)
{
    tk_t *tk = peek(file);
    // EOF or newline terminates a directive
    if (tk->type == TK_EOF || tk->prev_nl)
        return NULL;
    // Otherwise consume and return token
    adv(file);
    return tk;
}

static void process_params(file_t *file, map_t *name_to_index)
{
    tk_t *tk = next_on_line(file);
    if (tk == NULL)
        goto premature_end_of_params;

    // Empty parameter list
    if (tk->type == TK_RPAREN)
        return;

    for (size_t param_idx = 0;; ++param_idx) {
        // Add parameter to list
        bool found;
        entry_t *entry = map_find_or_insert(name_to_index, strdup(tk_str(tk)), &found);
        if (found)
            err("Duplicate macro parameter %s\n", tk_str(tk));
        entry->as_int = param_idx;

        // Read , or ) after parameter
        if ((tk = next_on_line(file)) == NULL)
            goto premature_end_of_params;

        if (tk->type == TK_RPAREN)
            return;
        if (tk->type != TK_COMMA)
            err("Expected , or ) instead of %s", tk_str(tk));

        // Read next parameter name
        if ((tk = next_on_line(file)) == NULL)
            goto premature_end_of_params;
    }

premature_end_of_params:
    err("Expected ) before end of line");
}

static void process_define(file_t *file, macro_vec_t *macros)
{
    tk_t *tk;

    if ((tk = next_on_line(file)) == NULL)
        err("Expected macro name before end of line");

    // Create macro
    macro_t *macro = macro_vec_push(macros);
    macro->name = strdup(tk_str(tk));
    macro->is_function = false;
    macro->is_variadic = false;
    macro->param_cnt = 0;
    tk_vec_init(&macro->replacement_list);

    // Parameter name to index mapping
    map_t name_to_index;
    map_init(&name_to_index);

    // Read first token
    tk = next_on_line(file);

    if (tk && tk->type == TK_LPAREN && tk->prev_white == false) {
        // Read parameter list
        process_params(file, &name_to_index);
        // FIXME: actually process variadic macros
        macro->is_function = true;
        macro->is_variadic = false;
        macro->param_cnt = name_to_index.count;
        // Read token after parameter list
        tk = next_on_line(file);
    }

    // Read macro body
    for (; tk; tk = next_on_line(file)) {

    }

    map_free(&name_to_index);
}

static void process_undef(file_t *file, macro_vec_t *macros)
{
    tk_t *name = next_on_line(file);

    if (name == NULL)
        err("Expected macro name before end of line");
}

static void process_directive(file_t *file, macro_vec_t *macros)
{
    tk_t *directive = next_on_line(file);

    // Empty directives are valid
    if (directive == NULL)
        return;

    // Otherwise we switch on the directive name
    if (strcmp(tk_str(directive), "define") == 0) {
        process_define(file, macros);
    } else if (strcmp(tk_str(directive), "undef") == 0) {
        process_undef(file, macros);
    } else {
        err("Unknown directive %s", tk_str(directive));
    }
}

static tk_t *next(file_t *file)
{
    tk_t *tk = peek(file);
    adv(file);
    return tk;
}

static bool match_lparen(file_t *file)
{
    tk_t *tk = peek(file);
    if (tk->type == TK_LPAREN) {
        adv(file);
        return true;
    }
    return false;
}

static void process_args(file_t *file)
{
    size_t paren_nest = 0;

    for (;;) {
        tk_t *tk = next(file);
        switch (tk->type) {
        case TK_LPAREN:
            ++paren_nest;
            break;
        case TK_RPAREN:
            if (paren_nest == 0)
                return;
            --paren_nest;
            break;
        case TK_COMMA:
            if (paren_nest == 0) {
                // TODO: goto next argument
                break;
            }
            // FALLTHROUGH
        default:
            // add token to current argument
            break;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    // Input file
    int in_fd = open(argv[1], O_RDONLY);
    if (in_fd < 0) {
        perror(argv[1]);
        return 1;
    }

    file_t file;
    file_init(&file, strdup(argv[1]), in_fd);

    // Macro database
    macro_vec_t macros;
    macro_vec_init(&macros);


    for (;;) {
        tk_t *tk = next(&file);

        // Exit if we've reached the end of file

        if (tk->type == TK_EOF) {
            break;
        }

        // Process directive

        if (tk->type == TK_HASH) {
            process_directive(&file, &macros);
            continue;
        }

        // Search for a matching macro

        macro_t *matching_macro = NULL;

        VEC_FOREACH(&macros, cur_macro) {
            if (strcmp(tk_str(tk), cur_macro->name) == 0) {
                matching_macro = cur_macro;
                break;
            }
        }

        if (matching_macro) {
            if (matching_macro->is_function) {
                if (match_lparen(&file)) {
                    // Function like macro
                    process_args(&file);
                    continue;
                }
            } else {
                // Object like macro

                continue;
            }
        }

        // Print token if no macro expansion is needed

        if (tk->prev_nl)
            putchar('\n');
        if (tk->prev_white)
            putchar(' ');

        printf("%s", tk_str(tk));
    }
}
