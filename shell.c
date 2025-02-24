#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void display_prompt() {
    /* Display the prompt before reading input */
    write(STDOUT_FILENO, "#cisfun$ ", 9);
}

char *read_input() {
    char *command = NULL;
    size_t len = 0;
    ssize_t nread;

    /* Use getline instead of fgets */
    nread = getline(&command, &len, stdin);
    if (nread == -1) {
        /* Handle EOF or error */
        free(command);
        return NULL;
    }
    
    /* Remove the newline character from input */
    if (nread > 0 && command[nread - 1] == '\n') {
        command[nread - 1] = '\0';
    }

    return command;
}

void execute_command(char *command) {
    char *argv[] = {command, NULL};  /* Array of arguments for execve */
    if (execve(command, argv, NULL) == -1) {  /* Try to execute the command */
        perror("execve");
    }
}

int main() {
    char *command;

    while (1) {
        display_prompt();  /* Display prompt before reading the command */
        command = read_input();  /* Read input from the user */

        if (command == NULL) {  /* Handle EOF (Ctrl+D) */
            printf("\n");
            break;
        }

        if (strlen(command) > 0) {
            execute_command(command);  /* Execute the command */
        }

        free(command);  /* Free the allocated memory for the command */
    }

    return 0;
}

