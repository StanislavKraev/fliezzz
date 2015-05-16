#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "creature.h"
#include "proto/iprotonode.h"

class GameManager: public IProtoNode
{
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
protected:
    virtual bool handleCommand(CommandType ctype, const CommandData &data);
private:
    QList<Creature*> m_creatures;
    IProtoMedia *m_protoMedia;
};

#endif // GAMEMANAGER_H
