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

    std::string stringLine = readString(STDIN_FILENO);

    if (checkPatternStr(stringLine)) {
        write(STDOUT_FILENO, stringLine.c_str(), stringLine.size() - 1);
        write(STDERR_FILENO, _USER_ALERT_VALID_OUT, sizeof(char) * _UAVO_SIZE);
    } else {
        write(STDERR_FILENO, _USER_ALERT_INVALID_OUT,sizeof(char) * _UAIO_SIZE);
    }

    return 0;
}