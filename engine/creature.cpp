#include "creature.h"

Creature::Creature()
{

}

Creature::~Creature()
{

}

void Creature::advance()
{
    // todo: reset mutex
}

void threadProc()
{
    // todo: - wait for mutex
    // todo: - get dt = now - last_timestamp.
    // todo: - calculate new state using AI and dt
    // todo: - during state calculation get game data from gamemanager cache
}
