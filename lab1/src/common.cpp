#include "common.h"

int readString(int fd, std::string & line)
{
    char symbol = ' ';
       
    while (true) {
        if (symbol == '\n') {
            break;
        }

        int res = read(fd, &symbol, sizeof(char));

        if (res == 0) {
            return EOF;
        }
        
        line.push_back(symbol);
    }

    return line.length();
}

void writeString(int fd, std::string line)
{
    int strLength = line.size();
    write(fd, line.c_str(), strLength);
}