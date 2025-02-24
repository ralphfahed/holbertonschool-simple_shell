#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 1024

/**
 * main - Simple shell implementation
 *
 * Return: 0 on success
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
        /* Read the command */
        nread = getline(&cmd, &len, stdin);

        /* Handle EOF (Ctrl+D) */
        if (nread == -1)
        {
            break;
        }

        /* Remove the newline character at the end */
        cmd[strcspn(cmd, "\n")] = '\0';

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

    /* Free allocated memory */
    free(cmd);
    return 0;
}

