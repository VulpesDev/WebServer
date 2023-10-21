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
    case 2:
        std::cerr << "Error: ";
    default:
        break;
    }
    std::cerr << str << std::endl;
    return errcode;
}