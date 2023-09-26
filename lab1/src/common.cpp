#include "common.h"

std::string readString(int fd)
{
    char symbol;
    std::string line;
    
    while (true) {
        if (symbol == '\n') 
            break;
        
        read(fd, &symbol, sizeof(char));
        line.push_back(symbol);
    }
    
    return line;
}

void writeString(int fd, std::string line)
{
    int strLength = line.size();
    write(fd, line.c_str(), strLength);
}