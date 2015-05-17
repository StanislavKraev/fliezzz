#ifndef CUSTOMEXCEPTIONS
#define CUSTOMEXCEPTIONS

#include <exception>

class InvalidProtocol : public std::exception
{
    virtual const char* what() const throw();
};

#endif // CUSTOMEXCEPTIONS

