#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QSet>
#include <QThread>

#include "engine/igamedataprovider.h"
#include "engine/gamestatus.h"
#include "proto/iprotonode.h"

class IProtoMedia;
class Creature;

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
protected:
    virtual bool handleCommand(CommandType ctype, const CommandData &data);

    virtual unsigned short getSize() const;
    virtual unsigned short getPointCapacity() const;
    virtual void getMovesFromPoint(const QPoint &pt, QSet<MoveDirection> &moveDirectionSet) const;
    virtual QPointF getFreeLandingPoint(const QPoint &pt) const;
    virtual bool isLandingPointFree(const QPointF &pt) const;
    virtual QUuid getCreatureAt(const QPointF &pt) const;
private:
    void getGameState(CommandData &data) const;
private:
    QList<Creature*> m_creatures;
    IProtoMedia *m_protoMedia;

    unsigned short m_fieldSize;
    unsigned short m_pointCapacity;
    double m_maxMoveTime;
    QSet<CommandType>  m_knownCommands;

    GameStatus m_status;
    bool m_shouldExit;
};

#endif // GAMEMANAGER_H
