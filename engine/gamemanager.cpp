#include <memory>

#include <QDebug>
#include <QDateTime>

#include "proto/customexceptions.h"
#include "proto/iprotomedia.h"

#include "engine/fly.h"
#include "engine/landingspot.h"
#include "engine/spotpart.h"
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

    qDebug() << QThread::currentThread()->currentThreadId();
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
//        qDebug() << "tick";
        CommandData gameState;
        this->getGameState(gameState);
        m_protoMedia->postCommand(CommandType::CtGameData, gameState);

        if (m_status == GameStatus::GsStarted)
        {
            double curTime = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000. - m_startTime;
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
    std::shared_ptr<QPointF> startPosition = getFreeLandingPoint(startPoint);
    if (!startPosition)
    {
        qWarning() << "Failed to add creature. Field occupied";
        return;
    }

//    double defaultMaxAge=60.; // s
//    double defaultMaxVelociy=0.1; // m/s/
//    double defaultMaxAlt=0.01; // m

    double maxAge = data[2].toDouble();
    double maxVelocity = data[3].toDouble();
    double maxAlt = data[4].toDouble();
    double maxThinkTime = data[5].toDouble();

    // todo: check max/min values

    Fly *newFly = new Fly(this, startPoint, *startPosition, maxAge, maxVelocity, maxAlt, maxThinkTime);
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
    if (pt.x() > 0)
    {
        if (getFreeLandingPoint(QPoint(pt.x() - 1, pt.y())))
        {
            moveDirectionSet.insert(MoveDirection::MdEast);
        }
        if (pt.y() > 0)
        {
            if (getFreeLandingPoint(QPoint(pt.x() - 1, pt.y() - 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdNorthEast);
            }
        }
        if (pt.y() < m_fieldSize - 1)
        {
            if (getFreeLandingPoint(QPoint(pt.x() - 1, pt.y() + 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdSouthEast);
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
            moveDirectionSet.insert(MoveDirection::MdWest);
        }
        if (pt.y() > 0)
        {
            if (getFreeLandingPoint(QPoint(pt.x() + 1, pt.y() - 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdNorthWest);
            }
        }
        if (pt.y() < m_fieldSize - 1)
        {
            if (getFreeLandingPoint(QPoint(pt.x() + 1, pt.y() + 1)))
            {
                moveDirectionSet.insert(MoveDirection::MdSouthWest);
            }
        }
    }
}

void GameManager::getSpotsOccupiedParts(LandingSpot *spot, QSet<unsigned short> partIds) const
{
    partIds.clear();

    QRectF spotRect = spot->getBBox();
    unsigned short spotPartsCount = spot->getPartsCount();
    // sync access to creature data
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
        unsigned short spotPartIndex = spot->getPartIndexFromPt(creaturePos);
        partIds.insert(spotPartIndex);
        if (partIds.count() == spotPartsCount)
        {
            return;
        }
    }
}

std::shared_ptr<QPointF> GameManager::getFreeLandingPoint(const QPoint &pt) const
{
    int pos = pt.y() * m_fieldSize + pt.x();
    if (pos >= m_field.count())
    {
        return nullptr; // todo: throw exception
    }
    LandingSpot *spot = m_field[pt.y() * m_fieldSize + pt.x()];
    QSet<unsigned short> partIds;
    getSpotsOccupiedParts(spot, partIds);
    unsigned short spotPartsCount = spot->getPartsCount();
    if (partIds.count() >= spotPartsCount)
    {
        return nullptr;
    }

    for (unsigned short i = 0; i < spotPartsCount; ++i)
    {
        if (!partIds.contains(i))
        {
            return std::make_shared<QPointF>(spot->getPart(i)->getCenter());
        }
    }

    return nullptr;
}

bool GameManager::isLandingPointFree(const QPointF &pt) const
{
    for (auto spot: m_field)
    {
        if (!spot->getBBox().contains(pt))
        {
            continue;
        }
        QSet<unsigned short> partIds;
        getSpotsOccupiedParts(spot, partIds);

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
                return true;
            }
        }
        return false;
    }
    return false;
}

QUuid GameManager::getCreatureAt(const QPointF &pt) const
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
    // todo: dynamic field reinit
    const double cellSize = 0.1;
    m_field.resize(m_fieldSize * m_fieldSize);
    for (unsigned int i = 0; i < m_fieldSize * m_fieldSize; ++i)
    {
        m_field[i] = new LandingSpot(i % m_fieldSize, i / m_fieldSize, cellSize, m_pointCapacity);
    }
}
