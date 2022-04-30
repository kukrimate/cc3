// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

static void write_asm(cc3_t *ctx, int outfd)
{
    // Generate text and data section
    dprintf(outfd, "section .text\n%s", ctx->gen.code.data);
    dprintf(outfd, "section .rodata\n%s", ctx->gen.lits.data);
    dprintf(outfd, "section .data\n%s", ctx->gen.data.data);

    // Generate imports and exports
    for (sym_t *sym = ctx->sema.scope->syms; sym; sym = sym->next)
        if (sym->kind == SYM_EXTERN)
            dprintf(outfd, "%s %s\n",
                sym->had_def ? "global" : "extern", sym->name);
}

static int run_process(char **args)
{
    pid_t pid = fork();
    if (pid < 0)                            // Failed to fork
        return -1;
    if (pid) {                              // Parent process
        int status;
        if (waitpid(pid, &status, 0) < 0)   // Failed to waitpid
            return -1;
        if (!WIFEXITED(status))             // Child didn't exit
            return -1;
        if ((errno = WEXITSTATUS(status)))  // Child reported error
            return -1;
        return 0;
    }
    execvp(args[0], args);                  // Child process
    exit(errno);                            // Exit if execvp fails
}

static int do_assemble(cc3_t *ctx, char *out_path)
{
    char asm_path[] = "/tmp/asmXXXXXX";
    int asm_fd = mkstemp(asm_path);

    if (asm_fd < 0)
        return -1;

    // Generate assembly
    write_asm(ctx, asm_fd);

    // Assemble
    char *args[] = { "nasm", "-felf64", "-o", out_path, asm_path, NULL };
    int err = run_process(args);

    close(asm_fd);
    unlink(asm_path);
    return err;
}

static int do_link(cc3_t *ctx, char *out_path)
{
    char obj_path[] = "/tmp/objXXXXXX";
    int obj_fd = mkstemp(obj_path);

    if (obj_fd < 0)
        return -1;

    // Run assembler
    int err = do_assemble(ctx, obj_path);

    // Run linker
    if (err == 0) {
        char *args[] = { "cc", "-no-pie", "-o", out_path, obj_path, NULL };
        err = run_process(args);
    }

    close(obj_fd);
    unlink(obj_path);
    return err;
}

int main(int argc, char **argv)
{
    int opt;
    bool opt_S = false;
    bool opt_c = false;
    char *output_path = NULL;

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

    int in_fd = open(argv[optind], O_RDONLY);

    if (in_fd < 0) {
        perror(argv[optind]);
        return 1;
    }

    // Setup context
    cc3_t ctx;
    cc3_init(&ctx, in_fd);

    // Parse file
    cc3_parse(&ctx);

    // Perform the actions requested by the user
    if (opt_S) {
        int out_fd = open(output_path, O_CREAT | O_WRONLY, 0666);
        if (out_fd < 0)
            goto err;
        write_asm(&ctx, out_fd);
        close(out_fd);
    } else if (opt_c) {
        if (do_assemble(&ctx, output_path) < 0)
            goto err;
    } else {
        if (do_link(&ctx, output_path) < 0)
            goto err;
    }

    cc3_free(&ctx);
    close(in_fd);
    return 0;

err:
    perror("cc3");
    cc3_free(&ctx);
    close(in_fd);
    return 1;
}
