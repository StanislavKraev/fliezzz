#include <QDebug>
#include <QDateTime>

#include "proto/customexceptions.h"
#include "proto/iprotomedia.h"

#include "engine/fly.h"
#include "engine/landingspot.h"
#include "engine/gamemanager.h"

GameManager::GameManager(QObject *parent, IProtoMedia *protoMedia):
    QThread(parent),
    m_protoMedia(protoMedia),
    m_fieldSize(10),
    m_pointCapacity(3),
    m_maxMoveTime(3.),
    m_status(GameStatus::GsStopped),
    m_shouldExit(false)
{
    Q_ASSERT(protoMedia);

    m_knownCommands.insert(CommandType::CtStartGame);
    m_knownCommands.insert(CommandType::CtStopGame);
    m_knownCommands.insert(CommandType::CtGetGameState);
    m_knownCommands.insert(CommandType::CtAddCreature);

    m_startTime = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000.;
    reinitField();
}

GameManager::~GameManager()
{
}

bool GameManager::handleCommand(CommandType ctype, const CommandData &data)
{
    switch(ctype)
    {
    case CommandType::CtStartGame:
        onStart();
        break;
    case CommandType::CtStopGame:
        onStop();
        break;
    case CommandType::CtGetGameState:
        onGetStatus();
        break;
    case CommandType::CtAddCreature:
        onAddCreature(data);
        break;
    default:
        return false;
    }
    return true;
}

void GameManager::onStart()
{
    qDebug() << "Game start";
    m_status = GameStatus::GsStarted;
}

void GameManager::onStop()
{
    m_status = GameStatus::GsStopped;
    qDebug() << "Game stop";
}

void GameManager::onGetStatus() const
{
    qDebug() << "Returning game status";
    CommandData data;
    data.append(QVariant(m_status));
    m_protoMedia->postCommand(CommandType::CtGameState, data);
}

void GameManager::onChangeConfig()
{

}

void GameManager::run()
{
    while (!m_shouldExit)
    {
        qDebug() << "tick";
        CommandData gameState;
        this->getGameState(gameState);
        m_protoMedia->postCommand(CommandType::CtGameData, gameState);

        if (m_status == GameStatus::GsStarted)
        {
            double curTime = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000. - m_startTime;
            qDebug() << curTime;
            for (auto fly: m_creatures)
            {
                fly->advance(curTime);
            }
        }

        m_protoMedia->canProcess(this, m_knownCommands);
        msleep(GameManager::CycleDelay);
    }
}

void GameManager::onAddCreature(const CommandData &data)
{
    if (data.count() != 6)
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
    //QPointF startPosition(data[2].toPointF());
    QPointF startPosition(getFreeLandingPoint(startPoint));

    double defaultMaxAge=60.; // s
    double defaultMaxVelociy=0.1; // m/s/
    double defaultMaxAlt=0.01; // m

    double maxAge = data[2].toDouble();
    double maxVelocity = data[3].toDouble();
    double maxAlt = data[4].toDouble();
    double maxThinkTime = data[5].toDouble();

    // todo: check max/min values

    Fly *newFly = new Fly(this, startPoint, startPosition, maxAge, maxVelocity, maxAlt, maxThinkTime);
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

QPoint GameManager::getPointByDirection(const QPoint &pt, MoveDirection moveDirection) const
{
    return pt; // TODO
}

void GameManager::reinitField()
{
    // todo: dynamic field reinit
    const double cellSize = 0.1;
    m_field.resize(m_fieldSize * 2);
    for (unsigned int i = 0; i < m_fieldSize * 2; ++i)
    {
        m_field[i] = new LandingSpot(i % m_fieldSize, i / m_fieldSize, cellSize, m_pointCapacity);
    }
}
