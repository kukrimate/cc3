#include "cc3.h"

int main(int argc, char **argv)
{
    struct cc3 ctx;

    cc3_init(&ctx, stdin);

    for (struct tk_buf *tk; (tk = lex_tok(&ctx, 0))->type; lex_adv(&ctx)) {
        lex_tok(&ctx, 1);
        printf("%s\n", tk_str(tk));
    }
}
