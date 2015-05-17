#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QTimer>

#include "engine/igamedataprovider.h"
#include "engine/gamestatus.h"
#include "proto/iprotonode.h"

class IProtoMedia;
class Creature;

// todo: move to separate thread
class GameManager: public QObject, public IProtoNode, public IGameDataProvider
{
    Q_OBJECT
public:
    GameManager(IProtoMedia *protoMedia);
    virtual ~GameManager();
public:
    static const int TimerDelay = 100;
protected:
    GameManager(const GameManager &);
    GameManager& operator=(const GameManager&);
protected:
    void start();
    void stop();
    void getStatus() const;
    void changeConfig();
    void addCreature(const CommandData &data);
protected:
    virtual bool handleCommand(CommandType ctype, const CommandData &data);

    virtual unsigned short getSize() const;
    virtual unsigned short getPointCapacity() const;
    virtual void getMovesFromPoint(const QPoint &pt, QSet<MoveDirection> &moveDirectionSet) const;
    virtual QPointF getFreeLandingPoint(const QPoint &pt) const;
    virtual bool isLandingPointFree(const QPointF &pt) const;
    virtual QUuid getCreatureAt(const QPointF &pt) const;
private slots:
    void tick();
private:
    void getGameState(CommandData &data) const;
private:
    QList<Creature*> m_creatures;
    IProtoMedia *m_protoMedia;

    unsigned short m_fieldSize;
    unsigned short m_pointCapacity;
    double m_maxMoveTime;

    QTimer *m_timer;
    GameStatus m_status;
};

#endif // GAMEMANAGER_H
