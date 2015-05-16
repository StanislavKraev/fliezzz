#include <QDebug>

#include "proto/iprotomedia.h"

#include "engine/gamemanager.h"

GameManager::GameManager(IProtoMedia *protoMedia): m_protoMedia(protoMedia)
{
    Q_ASSERT(protoMedia);

    QSet<CommandType> knownCommands = {CommandType::CtStartGame, CommandType::CtStopGame};
    m_protoMedia->subscribe(this, knownCommands);
}

GameManager::~GameManager()
{
    m_protoMedia->unsubscribe(this);
}

bool GameManager::handleCommand(CommandType ctype, const CommandData &data)
{
    switch(ctype)
    {
    case CommandType::CtStartGame:
        this->start();
        break;
    case CommandType::CtStopGame:
        this->stop();
        break;
    default:
        return false;
    }
    return true;
}

void GameManager::start()
{
    qDebug() << "Game start";
}

void GameManager::stop()
{
    qDebug() << "Game stop";
}

void GameManager::getStatus()
{

}

void GameManager::changeConfig()
{

}

