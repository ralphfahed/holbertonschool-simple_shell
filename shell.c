#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_INPUT_SIZE 1024

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[2];
    char *command = "/bin/ls";  /* Example: Always run ls for now */

    while (1) {
        /* Print prompt only if input is from the terminal (interactive) */
        if (isatty(fileno(stdin))) {
            printf("#cisfun$ ");
            fflush(stdout);
        }

        /* Read the input */
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break;  /* Exit if no input */
        }

        /* Remove trailing newline */
        input[strcspn(input, "\n")] = 0;

        /* Example: Handle the command '/bin/ls' */
        args[0] = command;
        args[1] = NULL;

        /* Execute the command */
        if (fork() == 0) {
            execve(command, args, NULL);
            perror("execve failed");
            exit(1);
        } else {
            wait(NULL);  /* Wait for child process */
        }
    }
    return 0;
}

