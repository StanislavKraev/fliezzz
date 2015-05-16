#ifndef IPROTOMEDIA
#define IPROTOMEDIA

#include <QSet>

#include "proto/commanddata.h"
#include "proto/commandtype.h"

class IProtoNode;

class IProtoMedia
{
public:
    virtual void subscribe(IProtoNode *protoNode, const QSet<CommandType> &supportedCommands) = 0;
    virtual void unsubscribe(IProtoNode *protoNode) = 0;
    virtual void handleCommand(CommandType command, const CommandData &data) = 0;
    virtual void handleCommand(CommandType command) = 0;
};

#endif // IPROTOMEDIA

