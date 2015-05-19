#include "proto/customexceptions.h"

namespace proto
{

const char* InvalidProtocol::what() const throw()
{
    return "Invalid protocol";
}

}
