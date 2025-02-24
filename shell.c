#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 1024

/**
 * main - A simple shell program that reads input and executes commands.
 *
 * Return: Always 0 (Success).
 */
int main(void)
{
    char *cmd = NULL;
    size_t len = 0;
    ssize_t nread;
    pid_t child_pid;
    int status;

    while (1)
    {
        /* Display the prompt */
        write(STDOUT_FILENO, "#cisfun$ ", 9);

        /* Read the command */
        nread = getline(&cmd, &len, stdin);

        /* Handle EOF (Ctrl+D) */
        if (nread == -1)
        {
            if (feof(stdin))  /* End of file reached */
            {
                write(STDOUT_FILENO, "\n", 1);
                break;
            }
            else
            {
                perror("getline");
                exit(1);
            }
        }

        /* Remove the newline character at the end */
        cmd[strcspn(cmd, "\n")] = '\0';

        /* Skip printing prompt when executing a command */
        if (strlen(cmd) == 0)
            continue;

        /* Fork a child process */
        child_pid = fork();
        if (child_pid == -1)  /* Fork failed */
        {
            perror("fork");
            free(cmd);
            exit(1);
        }

        if (child_pid == 0)  /* Child process */
        {
            /* Dynamically allocate an array for execve */
            char *args[2];  /* Only need one argument (cmd), plus NULL for execve */
            args[0] = cmd;
            args[1] = NULL;

            /* Execute the command using execve */
            if (execve(cmd, args, NULL) == -1)
            {
                /* Command not found */
                perror("./shell");
                free(cmd);
                exit(1);
            }
        }
        else  /* Parent process */
        {
            wait(&status);  /* Wait for the child to finish */
        }
    }

    /* Free the allocated memory */
    free(cmd);

    return 0;
}

