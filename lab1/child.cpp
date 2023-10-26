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

bool checkPatternStr(std::string string)
{
    int len = string.length();
    if (string[len - 2] == '.' || string[len - 2] == ';') {
        return true;
    }

    return false;
}

int main() {
    std::string stringLine;
    while (readString(STDIN_FILENO, stringLine) != EOF) {
        if (checkPatternStr(stringLine)) {
            write(STDERR_FILENO, stringLine.c_str(), stringLine.size());
            write(STDOUT_FILENO, _USER_ALERT_VALID_OUT, sizeof(char) * _UAVO_SIZE);
        } else {
            write(STDOUT_FILENO, _USER_ALERT_INVALID_OUT,sizeof(char) * _UAIO_SIZE);
        }
        stringLine.clear();
    }
    return 0;
}