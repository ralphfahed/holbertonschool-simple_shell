#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024

void execute_command(char *command, char **args) {
    if (access(command, F_OK) == -1) {
        write(STDERR_FILENO, "File does not exist\n", 20);
        exit(EXIT_FAILURE);
    }

    if (execve(command, args, NULL) == -1) {
        write(STDERR_FILENO, "execve failed\n", 14);
        exit(EXIT_FAILURE);
    }
}

void read_command(char *buffer) {
    int n = read(STDIN_FILENO, buffer, MAX_COMMAND_LENGTH - 1);
    if (n == -1) {
        write(STDERR_FILENO, "Error reading input\n", 20);
        exit(EXIT_FAILURE);
    } else if (n == 0) {
        exit(EXIT_SUCCESS);
    }

    buffer[n] = '\0';  
    if (buffer[n - 1] == '\n') {
        buffer[n - 1] = '\0';  
    }
}

int main(void) {
    char command[MAX_COMMAND_LENGTH];
    pid_t pid;

    while (1) {
        if (isatty(STDIN_FILENO)) {
            write(STDOUT_FILENO, "#cisfun$ ", 9);
        }

        read_command(command);

        if (strcmp(command, "exit") == 0) {
            break;
        }

        pid = fork();

        if (pid == -1) {
            write(STDERR_FILENO, "Fork failed\n", 12);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            char *args[MAX_COMMAND_LENGTH / 2 + 1];  
            char *cmd_token = strtok(command, " \n");
            int i = 0;

            while (cmd_token != NULL) {
                args[i] = cmd_token;
                i++;
                cmd_token = strtok(NULL, " \n");
            }
            args[i] = NULL;  

            execute_command(args[0], args);
        } else {
            wait(NULL);
        }
    }

    return 0;
}

