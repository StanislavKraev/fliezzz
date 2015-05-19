#ifndef IPROTOMEDIA
#define IPROTOMEDIA

#include <QSet>

#include "proto/commanddata.h"
#include "proto/commandtype.h"

namespace proto
{

class IProtoNode;

class IProtoMedia
{
public:
    virtual void canProcess(IProtoNode *protoNode, const QSet<CommandType> &supportedCommands) = 0;

    virtual void postCommand(CommandType command, const CommandData &data) = 0;
    virtual void postCommand(CommandType command) = 0;
};

}

#endif // IPROTOMEDIA

