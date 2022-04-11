#include "cc3.h"

int main(int argc, char **argv)
{
    struct cc3 ctx;

    cc3_init(&ctx, stdin);
    parse(&ctx);
}
