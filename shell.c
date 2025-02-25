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
char *find_command_in_path(const char *command) {
    char *path_env, *path_copy, *path, *full_path;
    
    path_env = getenv("PATH");
    if (!path_env || strlen(path_env) == 0) {
        return NULL;
    }
    
    path_copy = strdup(path_env); /* Create a copy of PATH to avoid modifying the original */
    if (!path_copy) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    
    path = strtok(path_copy, ":");
    full_path = NULL;
    
    while (path != NULL) {
        full_path = malloc(strlen(path) + strlen(command) + 2);
        if (!full_path) {
            perror("malloc");
            free(path_copy);
            exit(EXIT_FAILURE);
        }
        sprintf(full_path, "%s/%s", path, command);
        
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return full_path;
        }
        free(full_path);
        path = strtok(NULL, ":");
    }
    
    free(path_copy);
    return NULL;
}

void execute_command(char *command) {
    char *args[2];
    char *command_path;
    
    if (command[0] == '/' || command[0] == '.') {
        command_path = command;
    } else {
        command_path = find_command_in_path(command);
    }
    
    if (command_path == NULL) {
        write(STDERR_FILENO, "Command not found\n", 18);
        exit(127);
    }
    
    args[0] = command_path;
    args[1] = NULL;
    
    if (execve(command_path, args, environ) == -1) {
        perror("execve");
        exit(EXIT_FAILURE);
    }
}

void read_command(char *buffer) {
    int n;
    
    n = read(STDIN_FILENO, buffer, MAX_COMMAND_LENGTH - 1);
    if (n == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    } else if (n == 0) {
        exit(EXIT_SUCCESS);
    }
    
    buffer[n] = '\0';
    if (buffer[n - 1] == '\n') {
        buffer[n - 1] = '\0';
    }
}

void print_env(void) {
    char **env = environ;
    while (*env) {
        write(STDOUT_FILENO, *env, strlen(*env));
        write(STDOUT_FILENO, "\n", 1);
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
            exit(EXIT_SUCCESS);
        }
        
        if (strcmp(command, "env") == 0) {
            print_env();
            continue;
        }
        
        if (command[0] != '/' && command[0] != '.' && find_command_in_path(command) == NULL) {
            write(STDERR_FILENO, "Command not found\n", 18);
            continue;
        }
        
        pid = fork();
        if (pid == -1) {
            perror("fork");
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

