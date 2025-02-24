#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *args[1024];
    int i;
    
    while (1)
    {
        printf("$ ");
        nread = getline(&line, &len, stdin);

        /* Skip empty lines */
        if (nread == 1)
            continue;

        /* Remove newline character */
        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        /* Tokenize the input */
        i = 0;
        char *token = strtok(line, " ");
        while (token != NULL)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        /* Check if it's the 'exit' command */
        if (strcmp(args[0], "exit") == 0)
            break;

        /* Handle the 'env' command */
        if (strcmp(args[0], "env") == 0)
        {
            for (char **env = environ; *env != NULL; env++)
                printf("%s\n", *env);
            continue;
        }

        /* Handle executing commands */
        pid_t pid = fork();
        if (pid == 0)
        {
            char *command_path = get_command_path(args[0]);
            if (command_path == NULL)
            {
                perror("Command not found");
                exit(1);
            }
            execve(command_path, args, environ);
            perror("execve failed");
            exit(1);
        }
        else if (pid > 0)
        {
            wait(NULL);
        }
        else
        {
            perror("fork failed");
        }
    }

    free(line);
    return 0;
}

/* Function to find the command path */
char *get_command_path(char *command)
{
    char *path = getenv("PATH");
    if (path == NULL)
        return NULL;

    char *path_copy = strdup(path);
    if (path_copy == NULL)
        return NULL;

    char *dir = strtok(path_copy, ":");
    while (dir != NULL)
    {
        char *full_path = malloc(strlen(dir) + strlen(command) + 2);
        if (full_path == NULL)
        {
            free(path_copy);
            return NULL;
        }

        strcpy(full_path, dir);
        strcat(full_path, "/");
        strcat(full_path, command);

        if (access(full_path, F_OK) == 0)
        {
            free(path_copy);
            return full_path;
        }

        free(full_path);
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

