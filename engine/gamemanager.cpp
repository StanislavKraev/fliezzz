#include <QDebug>

#include "proto/customexceptions.h"
#include "proto/iprotomedia.h"

#include "engine/fly.h"
#include "engine/gamemanager.h"

GameManager::GameManager(IProtoMedia *protoMedia):
    m_protoMedia(protoMedia), m_timer(nullptr), m_status(GameStatus::GsStopped)
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
        start();
        break;
    case CommandType::CtStopGame:
        stop();
        break;
    case CommandType::CtGetGameState:
        this->getStatus();
        break;
    case CommandType::CtAddCreature:
        this->addCreature(data);
    default:
        return false;
    }
    return true;
}

void GameManager::start()
{
    qDebug() << "Game start";
    m_timer->start(GameManager::TimerDelay);
    m_status = GameStatus::GsStarted;
}

void GameManager::stop()
{
    m_timer->stop();
    m_status = GameStatus::GsStopped;
    qDebug() << "Game stop";
}

void GameManager::getStatus() const
{
    CommandData data;
    data.append(QVariant(m_status));
    m_protoMedia->handleCommand(CommandType::CtGameState, data);
}

void GameManager::changeConfig()
{

}

void GameManager::tick()
{
    //qDebug() << "tick";
    CommandData gameState;
    this->getGameState(gameState);
    m_protoMedia->handleCommand(CommandType::CtGameData, gameState);

    for (auto fly: m_creatures)
    {
        fly->advance();
    }
}

void GameManager::addCreature(const CommandData &data)
{
    // TODO: sync
    if (data.count() != 4)
    {
        throw new InvalidProtocol();
    }

    QString type = data[0].toString();
    if ("fly" != type)
    {
        qWarning() << "Failed to add creature. Unknown type: " << type;
        return;
    }

    QPoint startPoint(data[1].toPoint());
    QPointF startPosition(data[2].toPointF());

    double defaultMaxAge=60.; // s
    double defaultMaxVelociy=0.1; // m/s/
    double defaultMaxAlt=0.01; // m

    double maxAge = data[3].toDouble();
    double maxVelocity = data[4].toDouble();
    double maxAlt = data[5].toDouble();

    // todo: check max/min values

    Fly *newFly = new Fly(this, startPoint, startPosition, maxAge, maxVelocity, maxAlt);
    m_creatures.append(newFly);

    newFly->start();
}

void GameManager::getGameState(CommandData &data) const
{
    data.clear();
    data.reserve(m_creatures.count() * 4 + 4);
    data.append(QVariant(1)); // data type version
    data.append(QVariant(m_fieldSize));
    data.append(QVariant(m_pointCapacity));
    data.append(QVariant(m_creatures.count()));
    for (auto creature: m_creatures)
    {
        data.append(creature->getUid());
        data.append(creature->getType());
        data.append(creature->getPosition());
        data.append(creature->getState());
    }
}

unsigned short GameManager::getSize() const
{
    return m_fieldSize;
}

unsigned short GameManager::getPointCapacity() const
{
    return m_pointCapacity;
}

void GameManager::getMovesFromPoint(const QPoint &pt, QSet<MoveDirection> &moveDirectionSet) const
{
    moveDirectionSet.clear();
}

QPointF GameManager::getFreeLandingPoint(const QPoint &pt) const
{
    return QPointF(0., 0.);
}

bool GameManager::isLandingPointFree(const QPointF &pt) const
{
    return false;
}

QUuid GameManager::getCreatureAt(const QPointF &pt) const
{
    return QUuid();
}
