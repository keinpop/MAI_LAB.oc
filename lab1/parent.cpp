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

    std::string filename;
    if (readString(STDIN_FILENO, filename) == EOF) {
        writeString(STDOUT_FILENO, _USER_ALERT_ERROR_FILE);
        perror("file");
    }

    if ((fileFD = open(filename.c_str(), flags, mode)) < 0) {
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
        dup2(fileFD, STDERR_FILENO);

        execl("child.out", "child.out", NULL);
    } else { // parent process
        close(pipeFD1[RD]);
        close(pipeFD2[WR]);
        
        while (true) {
            writeString(STDOUT_FILENO, _USER_ALERT_STRING_INPUT);
            std::string str;
            if (readString(STDIN_FILENO, str) == EOF) {
                close(pipeFD1[WR]);
                break;
            }
            writeString(pipeFD1[WR], str);
            std::string response;
            
            readString(pipeFD2[RD], response);
            writeString(STDOUT_FILENO, response);
            
        }
        
        wait(NULL);

        close(pipeFD2[RD]);
    }
}