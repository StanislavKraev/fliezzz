#ifndef PROTOMEDIA_H
#define PROTOMEDIA_H

#include <QMultiMap>

#include "proto/iprotomedia.h"

class ProtoMedia: public IProtoMedia
{
public:
    ProtoMedia();
    virtual ~ProtoMedia();
public:
    virtual void subscribe(IProtoNode *protoNode, const QSet<CommandType> &supportedCommands);
    virtual void unsubscribe(IProtoNode *protoNode);
    virtual void handleCommand(CommandType command, const CommandData &data);
    virtual void handleCommand(CommandType command);
private:
    QMultiMap<CommandType, IProtoNode *> m_nodes;
};

#endif // PROTOMEDIA_H
