#include "main.h"

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
    char **array;

    (void)argc, (void)argv; /* Suppress unused parameter warnings */

    while (1)
    {
        write(STDOUT_FILENO, "MyShell$ ", 9);

        nread = getline(&buf, &count, stdin);
        if (nread == -1)
        {
            perror("Exiting shell");
            free(buf);
            exit(1);
        }

        array = malloc(sizeof(char *) * 1024);
        if (!array)
        {
            perror("Memory allocation failed");
            free(buf);
            exit(1);
        }

        token = strtok(buf, " \n");
        i = 0;
        while (token)
        {
            array[i] = token;
            token = strtok(NULL, " \n");
            i++;
        }
        array[i] = NULL;

        child_pid = fork();
        if (child_pid == -1)
        {
            perror("Failed to create process");
            free(array);
            free(buf);
            exit(41);
        }

        if (child_pid == 0)
        {
            if (execve(array[0], array, NULL) == -1)
            {
                perror("Failed to execute");
                free(array);
                free(buf);
                exit(97);
            }
        }
        else
        {
            wait(&status);
        }

        free(array);
    }
    
    free(buf);
    return (0);
}

