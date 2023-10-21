#include <iostream>
#include <string>
#include <cstring>
# define SPACES " \t\v\f"


enum    errCode{
    INFO,
    WARNING,
    ERROR
};

int err(std::string str, errCode err, int errcode);

std::string clearLeadingSpaces(std::string str);