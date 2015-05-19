#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QMutex>
#include <QSet>
#include <QThread>
#include <QVector>

#include "engine/igamedataprovider.h"
#include "engine/gamestatus.h"
#include "proto/iprotonode.h"

namespace proto
{
    class IProtoMedia;
}

namespace engine
{

class Creature;
class LandingSpot;

class GameManager: public QThread, public proto::IProtoNode, public IGameDataProvider
{
    Q_OBJECT
public:
    GameManager(QObject *parent, proto::IProtoMedia *protoMedia);
    virtual ~GameManager();
public:
    static const int CycleDelay = 100;
protected:
    GameManager(const GameManager &);
    GameManager& operator=(const GameManager&);
public:
    void run();
    void stopGame();
protected:
    void onStart();
    void onStop();
    void onGetStatus() const;
    void onGetConfig() const;
    void onChangeConfig();
    void onAddCreature(const proto::CommandData &data);
    void getSpotsOccupiedParts(LandingSpot *spot, QSet<unsigned short> &partIds) const;
protected:
    virtual bool handleCommand(proto::CommandType ctype, const proto::CommandData &data);

    virtual unsigned short getSize() const;
    virtual unsigned short getPointCapacity() const;
    virtual void getMovesFromPoint(const QPoint &pt, QSet<MoveDirection> &moveDirectionSet) const;
    virtual std::shared_ptr<QPointF> getFreeLandingPoint(const QPoint &pt) const;
    virtual bool isLandingPointFree(const QPointF &pt) const;
    virtual QUuid getCreatureAt(const QPointF &pt) const;
    virtual QPoint getPointByDirection(const QPoint &pt, MoveDirection moveDirection) const;
private:
    void getGameState(proto::CommandData &data) const;
    void reinitField();
private:
    QList<Creature*> m_creatures;
    proto::IProtoMedia *m_protoMedia;

    unsigned short m_fieldSize;
    unsigned short m_pointCapacity;
    double m_maxMoveTime;
    QSet<proto::CommandType>  m_knownCommands;

    GameStatus m_status;
    bool m_shouldExit;
    QVector<LandingSpot*> m_field;
    double m_startTime;
    mutable QMutex m_creatureMutex;
};

}

#endif // GAMEMANAGER_H
