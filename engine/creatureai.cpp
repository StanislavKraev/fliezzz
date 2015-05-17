#include "creatureai.h"

CreatureAI::CreatureAI(double maxAge, double maxVelocity, double maxAlt)
{

}

CreatureAI::~CreatureAI()
{

}

void CreatureAI::setState(const Creature::CreatureState &state)
{
    m_state = state;
}
