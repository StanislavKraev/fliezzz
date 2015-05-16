#ifndef CREATUREAI_H
#define CREATUREAI_H

#include "engine/creature.h"

class CreatureAI
{
public:
    CreatureAI(double maxAge, double maxVelocity, double maxAlt, const Creature::CreatureState &initialState);
    virtual ~CreatureAI();
public:
    void advance(double time, const CreatureState &newState);
};

#endif // CREATUREAI_H
