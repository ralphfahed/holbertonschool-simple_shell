#include "shell.h"

/**
  * main - Getline function
  * @argc: Argument count
  * @argv: Array of argument values
  *
  * Return: 0 on success
  */

int main(int argc, char **argv)
{
        (void)argc, (void)argv;
        char *buf = NULL, *token;
        size_t count = 0;
        ssize_t nread;
        pid_t child_pid;
        int i, status;
        char **array;

        while (1)
        {
                write(STDOUT_FILENO, "MyShell$ ", 9);

                nread = getline(&buf, &count, stdin);

                if (nread ==  -1)
                {
                        perror("Exiting shell");
                        exit(1);
                }

                token = strtok(buf, " \n");

                array = malloc(sizeof(char*) * 1024);
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
                        perror("Failed to create.");
                        exit (41);
                }

                if (child_pid == 0)
                {
                        if (execve(array[0], array, NULL) == -1)
                        {
                                perror("Failed to execute");
                                exit(97);
                        }
                }
                else
                {
                        wait(&status);
                }
        }
        free(buf);
        return (0);
}
