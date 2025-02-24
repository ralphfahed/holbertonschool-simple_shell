#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char **argv)
{
    char *buf = NULL;
    size_t count = 0;
    ssize_t nread;
    pid_t child_pid;
    int status;
    char *array[2];
    (void)argc, (void)argv;

    while (1) {
        write(STDOUT_FILENO, "MyShell$ ", 9);

        nread = getline(&buf, &count, stdin);

        if (nread == -1) {
            free(buf);
            return (0);
        }

        if (buf[nread - 1] == '\n') {
            buf[nread - 1] = '\0';
        }

        array[0] = strtok(buf, " \n");
        array[1] = NULL;

        if (array[0] == NULL) {
            continue;
        }

        if (strcmp(array[0], "ls") == 0) {
            array[0] = "/bin/ls";
        }

        child_pid = fork();
        if (child_pid == -1) {
            perror("Failed to create.");
            exit(41);
        }

        if (child_pid == 0) {
            if (execve(array[0], array, NULL) == -1) {
                perror("Command not found");
                exit(1);
            }
        } else {
            wait(&status);
        }
    }

    free(buf);
    return (0);
}

