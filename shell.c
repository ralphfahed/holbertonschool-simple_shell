#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS 64

extern char **environ;

char *get_command_path(char *command) {
    char *path = getenv("PATH");
    char *dir = strtok(path, ":");

    while (dir != NULL) {
        char *full_path = malloc(strlen(dir) + strlen(command) + 2);
        if (full_path == NULL) {
            perror("malloc");
            return NULL;
        }
        strcpy(full_path, dir);
        strcat(full_path, "/");
        strcat(full_path, command);

        if (access(full_path, X_OK) == 0) {
            return full_path;
        }

        free(full_path);
        dir = strtok(NULL, ":");
    }

    return NULL;
}

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *args[MAX_ARGS];

    while (1) {
        printf("$ ");
        nread = getline(&line, &len, stdin);

        if (nread == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        line[strcspn(line, "\n")] = 0;

        if (nread == 1) continue;  // Skip empty lines

        int i = 0;
        char *token = strtok(line, " ");
        while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Handle exit built-in
        if (strcmp(args[0], "exit") == 0) {
            exit(0);
        }

        // Handle env built-in
        if (strcmp(args[0], "env") == 0) {
            for (char **env = environ; *env != NULL; env++) {
                printf("%s\n", *env);
            }
            continue;
        }

        // Handle normal commands
        char *command_path = get_command_path(args[0]);
        if (command_path != NULL) {
            if (execve(command_path, args, environ) == -1) {
                perror("execve");
            }
            free(command_path);
        } else {
            fprintf(stderr, "./%s: %s: command not found\n", args[0], args[0]);
        }
    }

    free(line);
    return 0;
}

