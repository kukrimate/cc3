#include "cc3.h"

int main(int argc, char **argv)
{
    cc3_t ctx;
    lex_init(&ctx.lexer, stdin);
    sema_init(&ctx.sema);
    gen_init(&ctx.gen);

    // Parse file
    parse(&ctx);

    // Dump results
    printf("section .text\n%s", ctx.gen.code.data);
    printf("section .data\n%s", ctx.gen.data.data);
    // FIXME: dump exports and imports

    gen_free(&ctx.gen);
    sema_free(&ctx.sema);
    lex_free(&ctx.lexer);
}
