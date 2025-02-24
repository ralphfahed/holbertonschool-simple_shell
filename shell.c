#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT "#cisfun$ "

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    pid_t pid;

    while (1)
    {
        /* Prompt only printed when waiting for input */
        printf(PROMPT);
        fflush(stdout);
        nread = getline(&line, &len, stdin);

        if (nread == -1)
        {
            free(line);
            printf("\n");
            exit(0);
        }

        line[strcspn(line, "\n")] = 0;  /* Remove newline from input */
        if (strlen(line) == 0)
            continue;

        /* Check if the command exists and is executable */
        if (access(line, X_OK) != 0)
        {
            fprintf(stderr, "./shell: No such file or directory\n");
            continue;
        }

        pid = fork();
        if (pid == 0)  /* Child process */
        {
            char *args[2];
            args[0] = line;
            args[1] = NULL;

            execve(line, args, NULL);  /* Execute the command */
            perror("execve");
            exit(1);
        }
        else if (pid > 0)  /* Parent process */
        {
            wait(NULL);  /* Wait for the child process to finish */
        }
        else
        {
            perror("fork");
        }
    }

    return 0;
}

