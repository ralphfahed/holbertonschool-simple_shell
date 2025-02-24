#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char **argv)
{
    (void)argc, (void)argv;

    char *buf = NULL;
    size_t count = 0;
    ssize_t nread;
    pid_t child_pid;
    int status;
    char *array[2];  /* Array for command and argument */

    while (1)
    {
        write(STDOUT_FILENO, "MyShell$ ", 9);

        nread = getline(&buf, &count, stdin);

        if (nread == -1)
        {
            free(buf);
            return (0);  /* Exit gracefully on EOF */
        }

        /* Remove newline character if present */
        if (buf[nread - 1] == '\n')
        {
            buf[nread - 1] = '\0';
        }

        array[0] = strtok(buf, " \n");
        array[1] = NULL;

        if (array[0] == NULL)  /* Handle empty input (like pressing Enter) */
        {
            continue;
        }

        /* Check if the command is "ls" and prepend the full path */
        if (strcmp(array[0], "ls") == 0)
        {
            array[0] = "/bin/ls";
        }

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
                if (errno == ENOENT)
                {
                    perror("Command not found");
                }
                else if (errno == EACCES)
                {
                    perror("Permission denied");
                }
                else
                {
                    perror("Couldn't execute");
                }
                exit(7);  /* Exit child process if execve fails */
            }
        }
        else
        {
            wait(&status);  /* Parent waits for child to finish */

            if (WIFEXITED(status)) {
                printf("Child exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Child was terminated by signal %d\n", WTERMSIG(status));
            }
        }
    }

    free(buf);
    return (0);
}

