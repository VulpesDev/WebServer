#include "error.hpp"

int err(std::string str, errCode err, int errcode)
{
    switch (err)
    {
        case 0:
            std::cerr << "Info: ";
            break;
        case 1:
            std::cerr << "Warning: ";
            break;
        case 2:
            std::cerr << "Error: ";
            break;
        default:
            break;
    }
    std::cerr << str << std::endl;
    return errcode;
}

std::string clearLeadingSpaces(std::string str)
{
    size_t  i = 0;
    while (i < str.length() && std::strchr(SPACES, str.at(i))) {i++;}
    return &str.at(i);
}
