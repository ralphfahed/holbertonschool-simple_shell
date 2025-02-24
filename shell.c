#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 1024

extern char **environ;  /* The global environment variable */

/* Function to search for the command in the PATH directories */
char *find_command_in_path(char *command) {
    char *path_env = getenv("PATH");
    char *path;
    char *full_path;

    if (!path_env || strlen(path_env) == 0) {
        return NULL;
    }

    path = strtok(path_env, ":"); /* Get the first directory in PATH */
    while (path != NULL) {
        full_path = malloc(strlen(path) + strlen(command) + 2); /* Allocate memory for full path */
        if (!full_path) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        sprintf(full_path, "%s/%s", path, command); /* Build full path */

        if (access(full_path, X_OK) == 0) {
            return full_path;  /* Return the full path if found */
        }
        free(full_path);  /* Free memory if the command is not found in this directory */

        path = strtok(NULL, ":"); /* Move to the next directory */
    }

    return NULL; /* Return NULL if command is not found in any PATH directories */
}

void execute_command(char *command) {
    char *args[2];
    char *command_path;

    /* If the command contains a '/', treat it as an absolute or relative path */
    if (command[0] == '/' || command[0] == '.') {
        command_path = command;
    } else {
        command_path = find_command_in_path(command);
    }

    if (command_path == NULL) {
        write(STDERR_FILENO, "Command not found\n", 18);
        exit(2);  /* Return 2 if command is not found */
    }

    args[0] = command_path;
    args[1] = NULL;

    if (execve(command_path, args, NULL) == -1) {
        write(STDERR_FILENO, "execve failed\n", 14);
        exit(1);  /* Return 1 if execve fails */
    }
}

void read_command(char *buffer) {
    int n = read(STDIN_FILENO, buffer, MAX_COMMAND_LENGTH - 1);
    if (n == -1) {
        write(STDERR_FILENO, "Error reading input\n", 20);
        exit(EXIT_FAILURE);
    } else if (n == 0) {
        exit(EXIT_SUCCESS);
    }

    buffer[n] = '\0';  /* Ensure null termination */
    if (buffer[n - 1] == '\n') {
        buffer[n - 1] = '\0';  /* Remove the newline character */
    }
}

void print_env(void) {
    char **env = environ;  /* Access the global environment variable */
    
    while (*env) {
        write(STDOUT_FILENO, *env, strlen(*env));  /* Print each environment variable */
        write(STDOUT_FILENO, "\n", 1);  /* Print a new line after each variable */
        env++;
    }
}

int main(void) {
    char command[MAX_COMMAND_LENGTH];
    pid_t pid;

    while (1) {
        if (isatty(STDIN_FILENO)) {
            write(STDOUT_FILENO, "#cisfun$ ", 9);
        }

        read_command(command);

        if (strcmp(command, "exit") == 0) {
            exit(EXIT_SUCCESS);  /* Exit the shell with status 0 */
        }

        if (strcmp(command, "env") == 0) {
            print_env();  /* Print environment variables */
            continue;  /* Skip the fork and continue to the next command */
        }

        /* Check if the command exists in PATH before forking */
        if (find_command_in_path(command) == NULL && (command[0] != '/' && command[0] != '.')) {
            write(STDERR_FILENO, "Command not found\n", 18);
            continue;  /* Skip the fork and continue to next command */
        }

        pid = fork();

        if (pid == -1) {
            write(STDERR_FILENO, "Fork failed\n", 12);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            execute_command(command);
        } else {
            wait(NULL);
        }
    }

    return 0;
}

