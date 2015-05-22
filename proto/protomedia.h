#ifndef PROTOMEDIA_H
#define PROTOMEDIA_H

#include <QList>
#include <QMutex>
#include <QSet>

#include "proto/iprotomedia.h"

namespace proto
{

class ProtoMedia: public IProtoMedia
{
    static const unsigned short MaxQueueSize = 10;
public:
    typedef QPair<CommandType, CommandData> CmdDataPair;
public:
    ProtoMedia();
    virtual ~ProtoMedia();
public:
    virtual void canProcess(IProtoNode *protoNode, const QSet<CommandType> &supportedCommands);
    virtual void postCommand(CommandType command, const CommandData &data);
    virtual void postCommand(CommandType command);
private:
    QList< CmdDataPair > m_queue;
    QSet< CommandType > m_exclusiveCommands;
    QMutex m_mutex;
};

}

#endif // PROTOMEDIA_H
