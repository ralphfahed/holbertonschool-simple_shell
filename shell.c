#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

    /* Declare all variables at the top, before any code execution */
    char *buf = NULL;
    size_t count = 0;
    ssize_t nread;
    pid_t child_pid;
    int status;
    char *array[2];  /* Array for command and argument */

int main(int argc, char **argv)
{
    (void)argc, (void)argv;
    

    while (1)
    {
        /* Now the declaration is complete, you can safely write code here */
        write(STDOUT_FILENO, "#cisfun$ ", 9);

        nread = getline(&buf, &count, stdin);

        if (nread == -1)
        {
            perror("Exiting shell");
            exit(1);
        }

        /* Split the input into command and arguments */
        array[0] = strtok(buf, " \n");
        array[1] = NULL;

        child_pid = fork();

        if (child_pid == -1)
        {
            perror("Failed to create.");
            exit(41);
        }

        if (child_pid == 0)
        {
            if (execve(array[0], array, NULL) == -1)
            {
                perror("Couldn't execute");
                exit(7);
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

