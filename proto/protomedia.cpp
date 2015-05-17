#include <QDebug>

#include "proto/iprotonode.h"

#include "protomedia.h"

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
    m_mutex.unlock();
}

void ProtoMedia::canProcess(IProtoNode *protoNode, const QSet<CommandType> &supportedCommands)
{
    m_mutex.lock();
    auto cmdPairItBegin = m_queue.begin();
    auto cmdPairItEnd = m_queue.end();
    for (auto cmdPairIt = cmdPairItBegin; cmdPairIt != cmdPairItEnd; ++ cmdPairIt)
    {
        if (supportedCommands.find((*cmdPairIt).first) != supportedCommands.end())
        {
            CmdDataPair cdp = *cmdPairIt;
            m_queue.erase(cmdPairIt);
            m_mutex.unlock();

            if (!protoNode->handleCommand(cdp.first, cdp.second))
            {
                m_mutex.lock();
                m_queue.push_back(cdp);
                m_mutex.unlock();
            }
            return;
        }
    }
    m_mutex.unlock();
}
