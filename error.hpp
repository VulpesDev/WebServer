#include <iostream>
#include <string>

enum    errCode{
    INFO,
    WARNING,
    ERROR
};

int err(std::string str, errCode err, int errcode);