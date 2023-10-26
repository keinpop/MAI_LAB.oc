#pragma once

#include <iostream>
#include <string>
#include "stdio.h"
#include "unistd.h"
#include <cstdlib>
#include "sys/wait.h"
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>

#define RD 0
#define WR 1

#define _USER_ALERT_FILE_INPUT "Enter filename to work:\n"
#define _USER_ALERT_ERROR_FILE "\tFile is not opening\n"
#define _USER_ALERT_STRING_INPUT "Enter string to check:\n"
#define _USER_ALERT_INVALID_OUT "\tString isn't valid\n"
#define _UAIO_SIZE 21
#define _USER_ALERT_VALID_OUT "\tString is valid.\n"
#define _UAVO_SIZE 19

int readString(int fd, std::string & line);
void writeString(int fd, std::string line);