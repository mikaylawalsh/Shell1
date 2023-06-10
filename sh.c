#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
parse: parses the buffer string and populates the tokens, argv, filepath, and
redirection arrays. first parses the buffer by white space and fills each token
into the tokens array. Then separates the tokens array into redireciton
components (redirection symbols and files) and argv components (everything
else). lastly the full filepath of the command (the first argument of argv) is
loaded into the filepath array.
parameters: buffer, a string from the user populated in main; tokens, an empty
array of strings; argv, an empty array of strings; filepath, an empty array of
one string; and redirection, an empty array of strings.
returns: a int, -1 if there is an error or the input is empty; 0 otherwise.
*/
int parse(char buffer[1024], char *tokens[512], char *argv[512],
          char *filepath[1], char *redirection[512]) {
    int i = 0;
    char *token;
    token = strtok(buffer, "\t\n ");  // returns pointer to next token, used to
                                      // handle tabs, new lins, and spaces
    if (token == NULL) {
        return -1;
    }
    while (token != NULL) {
        tokens[i] = token;
        i++;
        token = strtok(0, "\t\n ");
    }

    int k = 0;
    int a = 0;
    int r = 0;
    int output = 0;
    int input = 0;
    while (tokens[k] != NULL) {
        if (!strcmp(tokens[k], ">") || !strcmp(tokens[k], ">>") ||
            !strcmp(tokens[k], "<")) {
            if (tokens[k + 1] == NULL) {
                if (!strcmp(tokens[k], "<")) {
                    fprintf(stderr, "syntax error: no input file.\n");
                } else {
                    fprintf(stderr, "syntax error: no output file.\n");
                }
                return -1;
            } else if (!strcmp(tokens[k + 1], "<")) {
                fprintf(stderr,
                        "syntax error: input file is redirection symbol.\n");
                return -1;
            } else if (!strcmp(tokens[k + 1], ">") ||
                       !strcmp(tokens[k + 1], ">>")) {
                fprintf(stderr,
                        "syntax error: output file is redirection symbol.\n");
                return -1;
            } else if ((!strcmp(tokens[k], ">") || !strcmp(tokens[k], ">>")) &&
                       output == 0) {
                output = 1;
                redirection[r] = tokens[k];  // fills redirection array
                redirection[r + 1] = tokens[k + 1];
                r += 2;  // these increase by two because we fill two indexes in
                         // the array above and then need to skip that the next
                         // iteration
                k += 2;
            } else if (!strcmp(tokens[k], "<") && input == 0) {
                input = 1;
                redirection[r] = tokens[k];
                redirection[r + 1] = tokens[k + 1];
                r += 2;
                k += 2;
            } else {
                fprintf(stderr, "syntax error: multiple output files\n");
                return -1;
            }
        } else {
            argv[a] = tokens[k];
            k++;
            a++;
        }
    }
    filepath[0] = argv[0];
    return 0;
}

/*
cd: checks syntax of input and changes directory if possible. calls and error
checks chdir in order to change directory.
parameters: argv, an array of strings containing the parsed input from the user.
returns: nothing, but prints out an error if necessary.
*/
void cd(char *argv[512]) {
    if (argv[1] == NULL) {
        fprintf(stderr, "cd: syntax error\n");
        return;
    }
    int c = chdir(argv[1]); /* chdir() changes the current working directory of
                               the calling process to the secified directory */
    if (c == -1) {
        fprintf(stderr, "cd: No such file or directory.\n");
    }
}

/*
ln: checks syntax of input and links inputs if possible. calls and error checks
link in order to change directory.
parameters: argv, an array of strings containing the parsed input from the user.
returns: nothing, but prints out an error if necessary.
*/
void ln(char *argv[512]) {
    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "ln: syntax error.\n");
        return;
    }
    int l =
        link(argv[1], argv[2]); /* link creates a new link to an existing file*/
    if (l == -1) {
        fprintf(stderr, "ln: No such file or directory.\n");
    }
}

/*
rm: checks syntax of input and unlinks input if possible. calls and error checks
unlink in order to change directory.
parameters: argv, an array of strings containing the parsed input from the user.
returns: nothing, but prints out an error if necessary.
*/
void rm(char *argv[512]) {
    if (argv[1] == NULL) {
        fprintf(stderr, "rm: syntax error.\n");
        return;
    }
    int u = unlink(argv[1]); /* unlink deletes a name from a file system. */
    if (u == -1) {
        fprintf(stderr, "rm: No such file or directory.\n");
    }
}

/*
handle_redirection: creates a child process, handles redirection (if needed) by
closing and opening desired file descriptors in order to print output or read
input. also updates argv[0] to be only the command instead of the entire
filepath. calls execv on the command and the arguments in argv. and lastly waits
for the child process to finish before continuing.
parameters: argv, an array of strings containing the parsed input from the user;
filepath, an array of one string containing the full filepath of the command;
and redirection, an array of strings containing any redirection symbols and
files from the user.
returns: nothing, but prints out an error if necessary.
*/
void handle_redirection(char *argv[512], char *filepath[1],
                        char *redirection[512]) {
    pid_t pid;
    pid = fork(); /* creates child process */
    if (pid == -1) {
        perror("pid");
        exit(1);
    } else if (pid == 0) {
        int i = 0;
        while (redirection[i] != NULL) {
            if (strcmp(redirection[i], ">") == 0) {
                if (close(1) == -1) {
                    perror("fd 1 failed closing");
                    exit(1);
                }
                if (open(redirection[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
                         0600) == -1) {
                    perror(redirection[i + 1]);
                    exit(1);
                }
            } else if (strcmp(redirection[i], "<") == 0) {
                if (close(0) == -1) {
                    perror("fd 0 failed closing");
                    exit(1);
                }
                if (open(redirection[i + 1], O_RDONLY) == -1) {
                    perror(redirection[i + 1]);
                    exit(1);
                }
            } else if (strcmp(redirection[i], ">>") == 0) {
                if (close(1) == -1) {
                    perror("fd 1 failed closing");
                    exit(1);
                }
                if (open(redirection[i + 1], O_CREAT | O_WRONLY | O_APPEND,
                         0600) == -1) {
                    perror(redirection[i + 1]);
                    exit(1);
                }
            }
            i++;
        }
        char *arg_one = strrchr(
            argv[0],
            (int)'/'); /* dealing with '/' in command. strchr returns pointer to
                          first occurance of charachter in given string */
        if (arg_one != NULL) {
            arg_one++;
            argv[0] = arg_one;
        }
        if (execv(filepath[0], argv) == -1) {
            perror("failure for execv to execute");
            exit(1);
        }
    }
    int w = waitpid(pid, NULL, 0); /* suspends execution of the calling process
                                      until child process changes state */
    if (w == -1) {
        perror("waitpid failed to execute");
        exit(1);
    }
}

/*
built_in: checks if argv[0] is one of the builtin commands. if so, it calls that
funtion; otherwise it calls handle_redirection.
parameters: argv, an array of strings containing the parsed input from the user;
filepath, an array of one string containing the full filepath of the command;
and redirection, an array of strings containing any redirection symbols and
files from the user.
returns: nothing.
*/
void built_in(char *argv[512], char *filepath[512], char *redirection[512]) {
    if (!strcmp(argv[0], "cd")) {
        cd(argv);
    } else if (!strcmp(argv[0], "ln")) {
        ln(argv);
    } else if (!strcmp(argv[0], "rm")) {
        rm(argv);
    } else if (!strcmp(argv[0], "exit")) {
        exit(0);
    } else {
        handle_redirection(argv, filepath, redirection);
    }
}

/*
main: prompts the user if PROMPT flag is set. reads in the input from the stdin
and puts it in a string, buffer. if read is successful and does not return 0
(meaning user entered Ctrl-D), then main calls parse on the buffer and the empty
arrays tokens, argv, filepath, and redirection. memset is called on all of these
arrays in order to fill it will 0's or NULLS originally. if parse returns 0,
meaning it was successful, built_in is called on argv, filepath, and
redirection. when this is done, the user is reprompted (if the PROMPT flag is
set). this loop will continue until the user enters exit or Ctrl-D.
parameters: nothing.
returns: an int, 0.
*/
int main(void) {
    while (1) {
#ifdef PROMPT
        if (printf("33sh> ") < 0) {
            fprintf(stderr, "printf prompt failed");
        }
        if (fflush(stdout) != 0) {
            perror("prompt printing failed");
        }
#endif

        size_t MAX = 1024;
        char buffer[MAX];
        memset(buffer, 0, MAX);
        char *tokens[512];
        memset(tokens, 0, 512 * sizeof(char *));
        char *argv[512];
        memset(argv, 0, 512 * sizeof(char *));
        char *filepath[1];
        memset(filepath, 0, 1 * sizeof(char *));
        char *redirection[512];
        memset(redirection, 0, 512 * sizeof(char *));

        if (read(0, buffer, MAX) != 0) {
            int p = parse(buffer, tokens, argv, filepath, redirection);
            if (p == 0) {
                built_in(argv, filepath, redirection);
            }
        } else {
            exit(1);
        }
    }
    return 0;
}