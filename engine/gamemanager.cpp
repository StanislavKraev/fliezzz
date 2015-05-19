#include <memory>

#include <QDebug>
#include <QDateTime>
#include <QMutexLocker>

#include "proto/customexceptions.h"
#include "proto/iprotomedia.h"

#include "engine/fly.h"
#include "engine/landingspot.h"
#include "engine/spotpart.h"
#include "engine/gamemanager.h"

using namespace proto;

namespace engine
{

GameManager::GameManager(QObject *parent, IProtoMedia *protoMedia):
    QThread(parent),
    m_protoMedia(protoMedia),
    m_fieldSize(10),
    m_pointCapacity(3),
    m_maxMoveTime(3.),
    m_status(GameStatus::GsStopped),
    m_shouldExit(false),
    m_startTime(0.)
{
    Q_ASSERT(protoMedia);

    m_knownCommands.insert(CommandType::CtStartGame);
    m_knownCommands.insert(CommandType::CtStopGame);
    m_knownCommands.insert(CommandType::CtGetGameState);
    m_knownCommands.insert(CommandType::CtAddCreature);
    m_knownCommands.insert(CommandType::CtGetConfig);
    m_knownCommands.insert(CommandType::CtChangeConfig);
    m_knownCommands.insert(CommandType::CtGetStats);

    reinitField();

    qDebug() << QThread::currentThread()->currentThreadId();
}

GameManager::~GameManager()
{
    m_shouldExit = true;
    m_status = GameStatus::GsStopped;
    QMutexLocker locker(&m_creatureMutex);
    for (auto creature: m_creatures)
    {
        delete creature;
    }
    for (auto spot: m_field)
    {
        delete spot;
    }
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
    case CommandType::CtGetConfig:
        onGetConfig();
        break;
    case CommandType::CtChangeConfig:
        onChangeConfig(data);
        break;
    case CommandType::CtGetStats:
        onGetStats();
        break;
    default:
        return false;
    }
    return true;
}

void GameManager::onStart()
{
    qDebug() << "Game start";
    m_startTime = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000.;
    m_status = GameStatus::GsStarted;
}

void GameManager::onStop()
{
    m_status = GameStatus::GsStopped;
    qDebug() << "Game stop";
}

void GameManager::onGetStatus() const
{
    CommandData data;
    data.append(QVariant(m_status));
    m_protoMedia->postCommand(CommandType::CtGameState, data);
}

void GameManager::onGetConfig() const
{
    CommandData data;
    data.append(QVariant(m_fieldSize));
    data.append(QVariant(m_pointCapacity));
    m_protoMedia->postCommand(CommandType::CtGameConfig, data);
}

void GameManager::onChangeConfig(const proto::CommandData &data)
{
    unsigned short newFieldSize = data.at(0).toUInt();
    unsigned short newSpotCapacity = data.at(1).toUInt();

    if (newFieldSize < 2 || newFieldSize > 40 || newSpotCapacity < 1 || newSpotCapacity > 20)
    {
        qWarning() << "Invalid config: " << newFieldSize << " & " << newSpotCapacity;
        return;
    }

    for (auto creature: m_creatures)
    {
        creature->stopAI();
    }

    QMutexLocker locker(&m_creatureMutex);
    for (auto creature: m_creatures)
    {
        delete creature;
    }
    m_creatures.clear();
    m_fieldSize = newFieldSize;
    m_pointCapacity = newSpotCapacity;
    m_status = GameStatus::GsStopped;
    reinitField();

    onGetStatus();
    onGetConfig();
}

void GameManager::run()
{
    while (!m_shouldExit)
    {
        CommandData gameState;
        this->getGameState(gameState);
        m_protoMedia->postCommand(CommandType::CtGameData, gameState);

        double curTime = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000.;
        double dt = m_startTime > 0 ? curTime - m_startTime : 0;
        if (m_status == GameStatus::GsStarted)
        {
            QMutexLocker locker(&m_creatureMutex);
            for (auto fly: m_creatures)
            {
                fly->advance(dt);
            }
        }

        m_protoMedia->canProcess(this, m_knownCommands);
        m_startTime = curTime;
        msleep(GameManager::CycleDelay);
    }

    for (auto creature: m_creatures)
    {
        creature->stopAI();
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
    qWarning() << "Adding creature to " << startPoint;
    std::shared_ptr<QPointF> startPosition = getFreeLandingPoint(startPoint);
    if (!startPosition)
    {
        qWarning() << "Failed to add creature. Field occupied";
        return;
    }

    double maxAge = data[2].toDouble();
    double maxVelocity = data[3].toDouble();
    double maxAlt = data[4].toDouble();
    double maxThinkTime = data[5].toDouble();

    {
        QMutexLocker locker(&m_creatureMutex);
        Fly *newFly = new Fly(this, startPoint, *startPosition, maxAge, maxVelocity, maxAlt, maxThinkTime);
        m_creatures.append(newFly);
        newFly->start();
    }
}

void GameManager::getGameState(CommandData &data) const
{
    QMutexLocker locker(&m_creatureMutex);
    data.clear();
    data.reserve(m_creatures.count() * 6 + 2);
    data.append(QVariant(4)); // data type version
    data.append(QVariant(m_creatures.count()));
    for (auto creature: m_creatures)
    {
        data.append(creature->getUid());
        data.append(creature->getType());
        data.append(creature->getPosition());
        data.append(creature->getState());
        data.append(creature->getAngle());
        data.append(creature->getMaxThinkTime());
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
    if (pt.x() > 0)
    {
        if (getFreeLandingPoint(QPoint(pt.x() - 1, pt.y())))
        {
            moveDirectionSet.insert(MoveDirection::MdWest);
        }
        if (pt.y() > 0)
        {
            if (getFreeLandingPoint(QPoint(pt.x() - 1, pt.y() - 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdNorthWest);
            }
        }
        if (pt.y() < m_fieldSize - 1)
        {
            if (getFreeLandingPoint(QPoint(pt.x() - 1, pt.y() + 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdSouthWest);
            }
        }
    }
    if (pt.y() < m_fieldSize - 1)
    {
        if (getFreeLandingPoint(QPoint(pt.x(), pt.y() + 1)))
        {
            moveDirectionSet.insert(MoveDirection::MdSouth);
        }
    }
    if (pt.y() > 0)
    {
        if (getFreeLandingPoint(QPoint(pt.x(), pt.y() - 1)))
        {
            moveDirectionSet.insert(MoveDirection::MdNorth);
        }
    }
    if (pt.x() < m_fieldSize - 1)
    {
        if (getFreeLandingPoint(QPoint(pt.x() + 1, pt.y())))
        {
            moveDirectionSet.insert(MoveDirection::MdEast);
        }
        if (pt.y() > 0)
        {
            if (getFreeLandingPoint(QPoint(pt.x() + 1, pt.y() - 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdNorthEast);
            }
        }
        if (pt.y() < m_fieldSize - 1)
        {
            if (getFreeLandingPoint(QPoint(pt.x() + 1, pt.y() + 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdSouthEast);
            }
        }
    }
}

void GameManager::getSpotsOccupiedParts(LandingSpot *spot, QSet<unsigned short> &partIds) const
{
    partIds.clear();

    QRectF spotRect = spot->getBBox();
    unsigned short spotPartsCount = spot->getPartsCount();
    QMutexLocker locker(&m_creatureMutex);
    for (auto creature: m_creatures)
    {
        if (creature->isMoving())
        {
            continue;
        }
        QPointF creaturePos = creature->getPosition();
        if (!spotRect.contains(creaturePos))
        {
            continue;
        }
        short spotPartIndex = spot->getPartIndexFromPt(creaturePos);
        if (spotPartIndex >= 0)
        {
            partIds.insert(spotPartIndex);
        }

        if (partIds.count() == spotPartsCount)
        {
            return;
        }
    }
}

std::shared_ptr<QPointF> GameManager::getFreeLandingPoint(const QPoint &pt) const
{
    //qDebug() << "getFreeLandingPoint " << pt;
    int pos = pt.y() * m_fieldSize + pt.x();
    if (pos >= m_field.count())
    {
        return nullptr; // todo: throw exception
    }
    LandingSpot *spot = m_field[pt.y() * m_fieldSize + pt.x()];
    QSet<unsigned short> partIds;
    getSpotsOccupiedParts(spot, partIds);
    //qDebug() << "getFreeLandingPoint occupied parts: " << partIds.count();
    unsigned short spotPartsCount = spot->getPartsCount();
    if (partIds.count() >= spotPartsCount)
    {
        //qDebug() << "getFreeLandingPoint occupied parts: all occupied";
        return nullptr;
    }

    QList<QPointF> freeParts;
    for (unsigned short i = 0; i < spotPartsCount; ++i)
    {
        if (!partIds.contains(i))
        {
            //qDebug() << "getFreeLandingPoint occupied parts: found empty";
            freeParts.append(spot->getPart(i)->getCenter());
        }
    }

    if (!freeParts.count())
    {
        return nullptr;
    }

    int choosenIndex = int((double)rand() / (double)RAND_MAX * (double)freeParts.count());
    auto partIt = freeParts.begin();
    while(choosenIndex > 0)
    {
        partIt ++;
        --choosenIndex;
    }
    return std::make_shared<QPointF>(*partIt);
}

bool GameManager::isLandingPointFree(const QPointF &pt) const
{
    //qDebug() << "isLandingPointFree " << pt;
    for (auto spot: m_field)
    {
        if (!spot->getBBox().contains(pt))
        {
            continue;
        }
        QSet<unsigned short> partIds;
        getSpotsOccupiedParts(spot, partIds);
        //qDebug() << "isLandingPointFree: occupied spot parts: " << partIds.count();

        unsigned short spotPartsCount = spot->getPartsCount();
        if (partIds.count() >= spotPartsCount)
        {
            return false;
        }

        for (unsigned short i = 0; i < spotPartsCount; ++i)
        {
            if (partIds.contains(i))
            {
                continue;
            }
            if (spot->getPart(i)->contains(pt))
            {
                //qDebug() << "isLandingPointFree: true ";
                return true;
            }
        }
        //qDebug() << "isLandingPointFree: false ";
        return false;
    }
    //qDebug() << "isLandingPointFree: false ";
    return false;
}

QUuid GameManager::getCreatureAt(const QPointF &) const
{
    return QUuid();
}

QPoint GameManager::getPointByDirection(const QPoint &pt, MoveDirection moveDirection) const
{
    switch (moveDirection)
    {
    case MoveDirection::MdNorth:
        return pt.y() > 0                               ? QPoint(pt.x(), pt.y() - 1) : pt;
    case MoveDirection::MdNorthEast:
        return pt.y() > 0 && pt.x() < m_fieldSize - 1   ? QPoint(pt.x() + 1, pt.y() - 1) : pt;
    case MoveDirection::MdEast:
        return pt.x() < m_fieldSize - 1                 ? QPoint(pt.x() + 1, pt.y()) : pt;
    case MoveDirection::MdSouthEast:
        return pt.x() < m_fieldSize - 1 && pt.y() < m_fieldSize - 1 ? QPoint(pt.x() + 1, pt.y() + 1) : pt;
    case MoveDirection::MdSouth:
        return pt.y() < m_fieldSize - 1                 ? QPoint(pt.x(), pt.y() + 1) : pt;
    case MoveDirection::MdSouthWest:
        return pt.y() < m_fieldSize - 1 && pt.x() > 0   ? QPoint(pt.x() - 1, pt.y() + 1) : pt;
    case MoveDirection::MdWest:
        return pt.x() > 0                               ? QPoint(pt.x() - 1, pt.y()) : pt;
    case MoveDirection::MdNorthWest:
        return pt.y() > 0 && pt.x() > 0                 ? QPoint(pt.x() - 1, pt.y() - 1) : pt;
    default:
        return pt;
    }

    return pt;
}

void GameManager::reinitField()
{
    if (m_field.count())
    {
        for (auto spot: m_field)
        {
            delete spot;
        }
    }
    m_field.clear();

    const double cellSize = 1.;
    m_field.resize(m_fieldSize * m_fieldSize);
    for (int i = 0; i < m_fieldSize * m_fieldSize; ++i)
    {
        m_field[i] = new LandingSpot(i % m_fieldSize, i / m_fieldSize, cellSize, m_pointCapacity);
    }
}

void GameManager::stopGame()
{
    m_status = GameStatus::GsStopped;
    m_shouldExit = true;
}

void GameManager::onGetStats() const
{
    QMutexLocker locker(&m_creatureMutex);
    CommandData data;
    data.reserve(m_creatures.count() * 4 + 1);
    data.append(QVariant(m_creatures.count()));
    for (auto creature: m_creatures)
    {
        data.append(creature->getUid());
        data.append(creature->getAverageVelocity());
        data.append(creature->getMileage());
        data.append(bool(creature->getState() == 100));
    }
    m_protoMedia->postCommand(CommandType::CtGameStats, data);
}

}
