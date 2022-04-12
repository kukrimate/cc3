#include "cc3.h"

int main(int argc, char **argv)
{
    struct cc3 ctx;
    lex_init(&ctx.lexer, stdin);
    sema_enter(&ctx.sema);
    parse(&ctx);
    sema_exit(&ctx.sema);
    lex_free(&ctx.lexer);
}
