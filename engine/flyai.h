#ifndef FLYAI_H
#define FLYAI_H

#include "engine/creatureai.h"

class FlyAI: public CreatureAI
{
public:
    FlyAI(double maxAge, double maxVelocity, double maxAlt);
    virtual ~FlyAI();
public:
    virtual void advance(double time, Creature::CreatureState &newState);
};

#endif // FLYAI_H
