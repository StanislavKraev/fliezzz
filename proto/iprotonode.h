#ifndef IPROTONODE
#define IPROTONODE

#include "proto/commanddata.h"
#include "proto/commandtype.h"

class IProtoNode {
public:
    virtual bool handleCommand(CommandType ctype, const CommandData &data) = 0;
protected:
    IProtoNode();
    ~IProtoNode();
private:
    IProtoNode(const IProtoNode &);
};

#endif // IPROTONODE

