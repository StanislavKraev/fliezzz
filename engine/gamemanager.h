#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QSet>
#include <QThread>
#include <QVector>

#include "engine/igamedataprovider.h"
#include "engine/gamestatus.h"
#include "proto/iprotonode.h"

class IProtoMedia;
class Creature;
class LandingSpot;

class GameManager: public QThread, public IProtoNode, public IGameDataProvider
{
    Q_OBJECT
public:
    GameManager(QObject *parent, IProtoMedia *protoMedia);
    virtual ~GameManager();
public:
    static const int CycleDelay = 100;
protected:
    GameManager(const GameManager &);
    GameManager& operator=(const GameManager&);
public:
    void run();
protected:
    void onStart();
    void onStop();
    void onGetStatus() const;
    void onChangeConfig();
    void onAddCreature(const CommandData &data);
    void getSpotsOccupiedParts(LandingSpot *spot, QSet<unsigned short> partIds) const;
protected:
    virtual bool handleCommand(CommandType ctype, const CommandData &data);

    virtual unsigned short getSize() const;
    virtual unsigned short getPointCapacity() const;
    virtual void getMovesFromPoint(const QPoint &pt, QSet<MoveDirection> &moveDirectionSet) const;
    virtual std::shared_ptr<QPointF> getFreeLandingPoint(const QPoint &pt) const;
    virtual bool isLandingPointFree(const QPointF &pt) const;
    virtual QUuid getCreatureAt(const QPointF &pt) const;
    virtual QPoint getPointByDirection(const QPoint &pt, MoveDirection moveDirection) const;
private:
    void getGameState(CommandData &data) const;
    void reinitField();
private:
    QList<Creature*> m_creatures;
    IProtoMedia *m_protoMedia;

    unsigned short m_fieldSize;
    unsigned short m_pointCapacity;
    double m_maxMoveTime;
    QSet<CommandType>  m_knownCommands;

    GameStatus m_status;
    bool m_shouldExit;
    QVector<LandingSpot*> m_field;
    double m_startTime;
};

#endif // GAMEMANAGER_H
