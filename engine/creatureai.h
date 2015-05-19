#ifndef CREATUREAI_H
#define CREATUREAI_H

#include "engine/creature.h"

namespace engine
{


class CreatureAI
{
public:
    CreatureAI(double maxAge, double maxVelocity, double maxAlt, const Creature::CreatureState &state);
    virtual ~CreatureAI();
public:
    virtual void init() = 0;
    void setState(const Creature::CreatureState &state);
    Creature::CreatureState getState() const;
protected:
    double m_maxAge;
    double m_maxVelocity;
    double m_maxAlt;
    Creature::CreatureState m_state;
    double m_curTime;
};

}

#endif // CREATUREAI_H
