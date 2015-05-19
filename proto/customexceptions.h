#ifndef CUSTOMEXCEPTIONS
#define CUSTOMEXCEPTIONS

#include <exception>

namespace proto
{

class InvalidProtocol : public std::exception
{
    virtual const char* what() const throw();
};

}

#endif // CUSTOMEXCEPTIONS

