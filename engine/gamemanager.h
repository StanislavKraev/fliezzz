#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QTimer>

#include "engine/creature.h"
#include "engine/igamedataprovider.h"
#include "proto/iprotonode.h"

class IProtoMedia;

class GameManager: public QObject, public IProtoNode, public IGameDataProvider
{
    Q_OBJECT
public:
    GameManager(IProtoMedia *protoMedia);
    virtual ~GameManager();
protected:
    GameManager(const GameManager &);
    GameManager& operator=(const GameManager&);
protected:
    void start();
    void stop();
    void getStatus();
    void changeConfig();
    void addFly();
protected:
    virtual bool handleCommand(CommandType ctype, const CommandData &data);
private slots:
    void tick();
private:
    QList<Creature*> m_creatures;
    IProtoMedia *m_protoMedia;

    unsigned short m_fieldSize;
    double m_maxMoveTime;

    QTimer *m_timer;
};

#endif // GAMEMANAGER_H
