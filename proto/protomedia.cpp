#include <QDebug>

#include "proto/iprotonode.h"

#include "protomedia.h"

ProtoMedia::ProtoMedia()
{

}

ProtoMedia::~ProtoMedia()
{
    if (!m_nodes.empty())
    {
        qWarning() << "Some nodes have not unsubscribed!";
    }
}

void ProtoMedia::subscribe(IProtoNode *protoNode, const QSet<CommandType> &supportedCommands)
{
    Q_ASSERT(protoNode);
    for (auto command: supportedCommands)
    {
        m_nodes.insert(command, protoNode);
    }
}

void ProtoMedia::unsubscribe(IProtoNode *protoNode)
{
// TODO
//    auto nodesBegin = m_nodes.begin();
//    auto nodesEnd = m_nodes.end();
//    for (auto nodeIt = nodesBegin; nodeIt != nodesEnd; ++nodeIt)
//    {
//        if (nodeIt == protoNode)
//        {
//            m_nodes.remove(node);
//        }
//    }
}

void ProtoMedia::handleCommand(CommandType command)
{
    CommandData emptyData;
    this->handleCommand(command, emptyData);
}

void ProtoMedia::handleCommand(CommandType command, const CommandData &data)
{
    auto targetNodes = m_nodes.values(command);
    for (int i = 0; i < targetNodes.size(); ++i)
    {
        if (targetNodes.at(i)->handleCommand(command, data))
        {
            return;
        }
    }
}
