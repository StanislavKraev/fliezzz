#include "proto/customexceptions.h"

const char* InvalidProtocol::what() const throw()
{
    return "Invalid protocol";
}
