#include "shell.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
  * main - Simple shell implementation using getline
  * @argc: Argument count
  * @argv: Array of argument values
  *
  * Return: 0 on success
  */
int main(int argc, char **argv)
{
    char *buf = NULL, *token;
    size_t count = 0;
    ssize_t nread;
    pid_t child_pid;
    int i, status;
    char *args[1024];  /* Array to store command and arguments */

    (void)argc, (void)argv; /* Suppress unused parameter warnings */

    while (1)
    {
        /* Only show the prompt if the shell is interactive */
        if (isatty(STDIN_FILENO)) {
            write(STDOUT_FILENO, "MyShell$ ", 9); /* Prompt */
        }

        nread = getline(&buf, &count, stdin);  /* Read input */
        if (nread == -1)
        {
            perror("Exiting shell");
            free(buf);
            exit(1);
        }

        /* Remove trailing newline character */
        buf[strcspn(buf, "\n")] = 0;

        /* Tokenize the input by space */
        i = 0;
        token = strtok(buf, " ");
        while (token != NULL)
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  /* Null-terminate the argument list */

        /* Check if the command is "exit" */
        if (strcmp(args[0], "exit") == 0)
        {
            free(buf);
            exit(0);
        }

        /* Fork a child process */
        child_pid = fork();
        if (child_pid == -1)  /* Fork failed */
        {
            perror("Fork failed");
            free(buf);
            exit(1);
        }
        if (child_pid == 0)  /* Child process */
        {
            if (execve(args[0], args, NULL) == -1)  /* Execute the command */
            {
                perror("execve failed");
                free(buf);
                exit(1);
            }
        }
        else  /* Parent process */
        {
            wait(&status);  /* Wait for the child process to finish */
        }
    }

    free(buf);
    return (0);
}

