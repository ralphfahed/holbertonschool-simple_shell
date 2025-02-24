#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    pid_t pid;
    int status;

    while (1)
    {
        /* Print the prompt before reading input */
        if (isatty(STDIN_FILENO))
        {
            printf("#cisfun$ ");
            fflush(stdout);
        }

        nread = getline(&line, &len, stdin);
        if (nread == -1)  /* Handle EOF (Ctrl+D) */
        {
            printf("\n");
            break;
        }

        line[strcspn(line, "\n")] = '\0';  /* Remove newline */

        if (strlen(line) == 0)  /* Ignore empty lines */
            continue;

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            continue;
        }

        if (pid == 0)  /* Child process */
        {
            char *argv[2];
            argv[0] = line;
            argv[1] = NULL;

            if (execve(line, argv, NULL) == -1)
            {
                perror("Error");
                exit(EXIT_FAILURE);
            }
        }
        else  /* Parent process */
        {
            wait(&status);  /* Parent waits for child */
        }
    }

    free(line);  /* Free allocated memory for line */
    return 0;
}

