#include <iostream>
#include <string>
#include "stdio.h"
#include "unistd.h"
#include <cstdlib>
#include "sys/wait.h"
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>

#include "src/common.h"

int main() {
    int pipeFD1[2];
    int pipeFD2[2];

    if (pipe(pipeFD1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipeFD2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int fileFD;
    mode_t mode = S_IRWXU;
    int flags = O_CREAT | O_WRONLY | O_APPEND;

    writeString(STDOUT_FILENO, _USER_ALERT_FILE_INPUT);

    if ((fileFD = open(readString(STDIN_FILENO).c_str(), flags, mode)) < 0) {
        writeString(STDOUT_FILENO, _USER_ALERT_ERROR_FILE);
        perror("file");
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("pid");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child process
        close(pipeFD2[RD]);
        close(pipeFD1[WR]);
        
        dup2(pipeFD2[WR], STDOUT_FILENO);
        dup2(pipeFD1[RD], STDIN_FILENO);
        dup2(fileFD, STDOUT_FILENO);

        execl("child.out", "child.out", NULL);
    } else { // parent process
        close(pipeFD1[RD]);
        close(pipeFD2[WR]);

        std::string input;
        
        writeString(STDOUT_FILENO, _USER_ALERT_STRING_INPUT);
        writeString(pipeFD1[WR], readString(STDIN_FILENO));
        writeString(STDOUT_FILENO, readString(pipeFD2[RD]));

        wait(NULL);

        close(pipeFD1[WR]);
        close(pipeFD2[RD]);
    }
}