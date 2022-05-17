// SPDX-License-Identifier: GPL-2.0-only

#include "cc3.h"

static int run_cpp(char *in_path)
{
    // Create pipe
    int pipefd[2];
    if (pipe(pipefd) < 0)
        return -1;

    // Fork child process
    pid_t pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    if (pid) {
        // Parent process
        close(pipefd[1]);
        return pipefd[0];
    } else {
        // Child process
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = { "cpp", "-U__GNUC__", "-P", in_path, NULL };
        execvp(args[0], args);
        exit(errno);
    }
}

static int run_command(char **args)
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

static int do_compile(int in_fd, char *out_path)
{
    int out_fd = open(out_path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (out_fd < 0)
        return -1;
    cc3_compile(in_fd, out_fd);
    close(out_fd);
    return 0;
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
    int err = run_command(args);

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
        err = run_command(args);
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
    bool opt_r = false;
    char *out_path = NULL;

    while ((opt = getopt(argc, argv, "Scro:h")) != -1)
        switch (opt) {
        case 'S':   // Generate assembly
            opt_S = true;
            break;
        case 'c':   // Generate object file
            opt_c = true;
            break;
        case 'r':   // Read raw input
            opt_r = true;
            break;
        case 'o':   // Output file
            out_path = optarg;
            break;
        case 'h':
        default:
        usage:
            fprintf(stderr, "Usage: %s [-Sc] -o OUTPUT INPUT\n", argv[0]);
            return 1;
        }

    if (!out_path)
        goto usage;

    if (optind >= argc)
        goto usage;

    // Run pre-processor or read raw input

    int in_fd;

    if (opt_r)
        in_fd = open(argv[optind], O_RDONLY);
    else
        in_fd = run_cpp(argv[optind]);

    if (in_fd < 0)
        goto err;

    // Perform the actions requested by the user
    
    if (opt_S) {
        if (do_compile(in_fd, out_path) < 0)
            goto err;
    } else if (opt_c) {
        if (do_assemble(in_fd, out_path) < 0)
            goto err;
    } else {
        if (do_link(in_fd, out_path) < 0)
            goto err;
    }

    return 0;

err:
    perror("cc3");
    return 1;
}
