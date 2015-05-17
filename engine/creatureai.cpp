#include "creatureai.h"

CreatureAI::CreatureAI(double maxAge, double maxVelocity, double maxAlt, const Creature::CreatureState &state) :
    m_curTime(-1.),
    m_maxAge(maxAge),
    m_maxVelocity(maxVelocity),
    m_maxAlt(maxAlt),
    m_state(state)
{

}

CreatureAI::~CreatureAI()
{

}

void CreatureAI::setState(const Creature::CreatureState &state)
{
    m_state = state;
}
