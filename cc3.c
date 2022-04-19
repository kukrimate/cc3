#include "cc3.h"

static void cc3_init(cc3_t *ctx, FILE *input_file)
{
    lex_init(&ctx->lexer, input_file);
    sema_init(&ctx->sema);
    gen_init(&ctx->gen);
}

static void cc3_free(cc3_t *ctx)
{
    lex_free(&ctx->lexer);
    sema_free(&ctx->sema);
    gen_free(&ctx->gen);
}

static void cc3_write_asm(cc3_t *ctx, FILE *output_file)
{
    fprintf(output_file, "section .text\n%s", ctx->gen.code.data);
    fprintf(output_file, "section .data\n%s", ctx->gen.data.data);
    // FIXME: dump exports and imports
}

static int do_assemble(cc3_t *ctx, const char *out_path)
{
    int err = 0;

    char asm_path[L_tmpnam];
    if (!tmpnam(asm_path)) {
        err = -1;
        goto done;
    }

    // Write assembly to tempfile
    FILE *asm_file = fopen(asm_path, "w");
    if (!asm_file) {
        err = -1;
        goto done;
    }
    cc3_write_asm(ctx, asm_file);
    fclose(asm_file);

    // Run assembler
    char cmd[4096];
    if (snprintf(cmd, sizeof cmd, "nasm -felf64 -o %s %s", out_path, asm_path) >= sizeof cmd) {
        err = -1;
        goto done;
    }

    int ret = system(cmd);
    if (!WIFEXITED(ret) || WEXITSTATUS(ret))
        err = -1;

done:
    unlink(asm_path);
    return err;
}

static int do_link(cc3_t *ctx, const char *out_path)
{
    int err = 0;

    char obj_path[L_tmpnam];
    if (!tmpnam(obj_path)) {
        err = -1;
        goto done;
    }

    // Assemble to temporary file
    if (do_assemble(ctx, obj_path) < 0) {
        err = -1;
        goto done;
    }

    // Run linker
    char cmd[4096];
    if (snprintf(cmd, sizeof cmd, "cc -o %s %s", out_path, obj_path) >= sizeof cmd) {
        err = -1;
        goto done;
    }

    int ret = system(cmd);
    if (!WIFEXITED(ret) || WEXITSTATUS(ret))
        err = -1;

done:
    unlink(obj_path);
    return err;
}

int main(int argc, char **argv)
{
    int opt;
    bool opt_S = false;
    bool opt_c = false;
    const char *output_path = NULL;
    FILE *input_file = NULL;

    while ((opt = getopt(argc, argv, "Sco:h")) != -1)
        switch (opt) {
        case 'S':   // Generate assembly
            opt_S = true;
            break;
        case 'c':   // Generate assembly and assemble
            opt_c = true;
            break;
        case 'o':   // Output file
            output_path = optarg;
            break;
        case 'h':
        default:
        usage:
            fprintf(stderr, "Usage: %s [-Sc] -o OUTPUT INPUT\n", argv[0]);
            return 1;
        }

    if (!output_path)
        goto usage;

    if (optind >= argc)
        goto usage;

    if (!(input_file = fopen(argv[optind], "r"))) {
        perror(argv[optind]);
        return 1;
    }

    // Setup context
    cc3_t ctx;
    cc3_init(&ctx, input_file);

    // Parse file
    parse(&ctx);

    // Perform the actions requested by the user
    if (opt_S) {
        FILE *output_file;
        if (!(output_file = fopen(output_path, "w"))) {
            perror(output_path);
            goto err;
        }
        cc3_write_asm(&ctx, output_file);
        fclose(output_file);
    } else if (opt_c) {
        if (do_assemble(&ctx, output_path) < 0)
            goto err;
    } else {
        if (do_link(&ctx, output_path) < 0)
            goto err;
    }

    cc3_free(&ctx);
    fclose(input_file);
    return 0;

err:
    cc3_free(&ctx);
    fclose(input_file);
    return 1;
}
