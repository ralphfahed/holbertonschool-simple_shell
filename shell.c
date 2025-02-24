#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    char *buf = NULL;
    size_t count = 0;
    ssize_t nread;
    char *array[2];
    pid_t child_pid;
    int status;

    while (1) {
        /* Step 1: Display the prompt */
        write(STDOUT_FILENO, "#cisfun$ ", 9);

        /* Step 2: Read the user input */
        nread = getline(&buf, &count, stdin);

        /* Step 3: Check for EOF (exit if so) */
        if (nread == -1) {
            free(buf);
            return (0);  /* Exit gracefully on EOF */
        }

        /* Step 4: Remove the newline character */
        if (buf[nread - 1] == '\n') {
            buf[nread - 1] = '\0';
        }

        /* Step 5: Tokenize the input */
        array[0] = strtok(buf, " \n");
        array[1] = NULL;

        /* Step 6: Handle empty input */
        if (array[0] == NULL) {
            continue;  /* If input is empty, go back to the top of the loop */
        }

        /* Step 7: Handle built-in 'exit' command */
        if (strcmp(array[0], "exit") == 0) {
            free(buf);
            exit(0);  /* Exit gracefully */
        }

        /* Step 8: Handle command execution */
        child_pid = fork();
        if (child_pid == -1) {
            perror("Failed to create process");
            exit(41);
        }

        if (child_pid == 0) {  /* Child process */
            if (execve(array[0], array, NULL) == -1) {
                perror("Command not found");
                exit(1);
            }
        } else {  /* Parent process */
            wait(&status);  /* Wait for child to finish */
        }
    }

    free(buf);
    return 0;
}

