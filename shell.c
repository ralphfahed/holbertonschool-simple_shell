i#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFER_SIZE 1024

extern char **environ;  /* Declare environ */

/* Function to display the prompt */
void display_prompt() {
    write(1, "#cisfun$ ", 9); /* Write the prompt to stdout */
}

/* Function to read a line of input */
char *read_input() {
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
        free(buffer);
        return NULL; /* EOF detected */
    }
    return buffer;
}

/* Function to execute a command */
void execute_command(char *command) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  /* Child process */
        char *argv[2];  /* Declare argv here */
        argv[0] = command;  /* Set the command */
        argv[1] = NULL;  /* Null terminate the argument list */
        if (execve(command, argv, environ) == -1) {  /* Try to execute the command */
            perror("./shell");  /* Print error message if execve fails */
            exit(EXIT_FAILURE);
        }
    } else {  /* Parent process */
        wait(NULL);  /* Wait for the child process to finish */
    }
}

int main() {
    char *command;
    size_t len;  /* Declare len at the top */

    while (1) {
        display_prompt();  /* Display prompt */
        command = read_input();  /* Read input from the user */

        if (command == NULL) {  /* Handle EOF (Ctrl+D) */
            printf("\n");
            break;
        }

        /* Remove newline character if present */
        len = strlen(command);  /* Now it's declared at the top */
        if (len > 0 && command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }

        /* If the command is not empty, attempt to execute it */
        if (strlen(command) > 0) {
            execute_command(command);
        }

        free(command);  /* Free the allocated memory for the command */
    }

    return 0;
}

