#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>  /* Include this to declare wait() */

#define MAX_CMD_LEN 1024

void execute_command(char *command) {
    pid_t pid = fork();

    if (pid == 0) {
        /* In child process */
        char *args[2];  /* Create fixed array */
        args[0] = command;
        args[1] = NULL;  /* Set last element to NULL for execve */

        if (execve(command, args, NULL) == -1) {
            perror("execve");  /* If execution fails */
            exit(1);
        }
    } else if (pid > 0) {
        /* In parent process */
        wait(NULL);  /* Wait for the child process to finish */
    } else {
        /* Fork failed */
        perror("fork");
    }
}

int main() {
    char input[MAX_CMD_LEN];
    char *cmd;  /* Declare cmd variable before code */

    /* Read input from stdin */
    while (1) {
        printf("#cisfun$ ");
        if (fgets(input, MAX_CMD_LEN, stdin) == NULL) {
            break;  /* Exit if error reading input */
        }

        /* Remove trailing newline character */
        input[strcspn(input, "\n")] = 0;

        /* Check if input is not empty */
        if (strlen(input) == 0) {
            continue;  /* Skip empty input */
        }

        /* Split the input by newline to handle multiple commands */
        cmd = strtok(input, "\n");

        while (cmd != NULL) {
            execute_command(cmd);
            cmd = strtok(NULL, "\n");
        }
    }

    return 0;
}

