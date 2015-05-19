#include <QDebug>

#include "proto/iprotonode.h"

#include "protomedia.h"

namespace proto
{

ProtoMedia::ProtoMedia()
{

}

ProtoMedia::~ProtoMedia()
{
}

void ProtoMedia::postCommand(CommandType command)
{
    CommandData emptyData;
    postCommand(command, emptyData);
}

void ProtoMedia::postCommand(CommandType command, const CommandData &data)
{
    m_mutex.lock();
    m_queue.push_back(CmdDataPair(command, data));

    if (m_queue.count() > ProtoMedia::MaxQueueSize)
    {
        QMutableListIterator<CmdDataPair> it(m_queue);
        while (it.hasNext() && m_queue.count() > ProtoMedia::MaxQueueSize)
        {
            if (it.next().first > CommandType::CtObsoletableCommands)
            {
                it.remove();
            }
        }
    }

    if (m_queue.count() > ProtoMedia::MaxQueueSize)
    {
        qWarning() << "protocol queue is overfull";
    }
    m_mutex.unlock();
}

void ProtoMedia::canProcess(IProtoNode *protoNode, const QSet<CommandType> &supportedCommands)
{
    m_mutex.lock();
    auto cmdPairItBegin = m_queue.begin();
    auto cmdPairItEnd = m_queue.end();
    for (auto cmdPairIt = cmdPairItBegin; cmdPairIt != cmdPairItEnd; ++cmdPairIt)
    {
        if (supportedCommands.contains((*cmdPairIt).first))
        {
            CmdDataPair cdp = *cmdPairIt;
            m_queue.erase(cmdPairIt);
            m_mutex.unlock();

            if (!protoNode->handleCommand(cdp.first, cdp.second))
            {
                qDebug() << "returning back to queue (has not processed): " << (*cmdPairIt).first;
                m_mutex.lock();
                m_queue.push_back(cdp);
                m_mutex.unlock();
            }
            return;
        }
    }
    m_mutex.unlock();
}

}
