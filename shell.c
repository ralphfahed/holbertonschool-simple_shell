#include "shell.h"

/**
 * main - Simple UNIX command line interpreter
 * Return: 0 on success, 1 on failure
 */
int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    pid_t pid;
    int status;

    while (1)
    {
        printf("$ ");
        nread = getline(&line, &len, stdin);

        if (nread == -1) /* Handle EOF (Ctrl+D) */
        {
            printf("\n");
            break;
        }

        line[strcspn(line, "\n")] = '\0'; /* Remove newline */

        if (strlen(line) == 0) /* Ignore empty input */
            continue;

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            execute_command(line);
        }
        else
        {
            wait(&status);
        }
    }

    free(line);
    return (0);
}

/**
 * execute_command - Executes a command
 * @cmd: The command to execute
 */
void execute_command(char *cmd)
{
    char *args[] = {cmd, NULL};

    execve(args[0], args, environ);
    perror("execve"); /* Print error if execve fails */
    exit(EXIT_FAILURE);
}
