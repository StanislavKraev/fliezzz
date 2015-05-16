#include <QDebug>

#include "proto/iprotomedia.h"

#include "engine/gamemanager.h"

GameManager::GameManager(IProtoMedia *protoMedia): m_protoMedia(protoMedia), m_timer(nullptr)
{
    Q_ASSERT(protoMedia);

    QSet<CommandType> knownCommands = {CommandType::CtStartGame, CommandType::CtStopGame};
    m_protoMedia->subscribe(this, knownCommands);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

GameManager::~GameManager()
{
    delete m_timer;
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
    m_timer->start(100);
}

void GameManager::stop()
{
    m_timer->stop();
    qDebug() << "Game stop";
}

void GameManager::getStatus()
{

}

void GameManager::changeConfig()
{

}

void GameManager::tick()
{
    //qDebug() << "tick";
    // TODO: анимация будет отставать на 1 tick и повторять действия мух
}
