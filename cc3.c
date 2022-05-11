// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

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

static int do_assemble(int in_fd, char *out_path)
{
    char asm_path[] = "/tmp/asmXXXXXX";
    int asm_fd = mkstemp(asm_path);

    if (asm_fd < 0)
        return -1;

    // Generate assembly
    cc3_compile(in_fd, asm_fd);

    // Assemble
    char *args[] = { "as", "-o", out_path, asm_path, NULL };
    int err = run_process(args);

    close(asm_fd);
    unlink(asm_path);
    return err;
}

static int do_link(int in_fd, char *out_path)
{
    char obj_path[] = "/tmp/objXXXXXX";
    int obj_fd = mkstemp(obj_path);

    if (obj_fd < 0)
        return -1;

    // Run assembler
    int err = do_assemble(in_fd, obj_path);

    // Run linker
    if (err == 0) {
        char *args[] = { "cc", "-o", out_path, obj_path, NULL };
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


    // Perform the actions requested by the user
    if (opt_S) {
        int out_fd = open(output_path, O_CREAT | O_WRONLY, 0666);
        if (out_fd < 0)
            goto err;
        cc3_compile(in_fd, out_fd);
        close(out_fd);
    } else if (opt_c) {
        if (do_assemble(in_fd, output_path) < 0)
            goto err;
    } else {
        if (do_link(in_fd, output_path) < 0)
            goto err;
    }

    close(in_fd);
    return 0;

err:
    perror("cc3");
    close(in_fd);
    return 1;
}
