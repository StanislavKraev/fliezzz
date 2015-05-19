#include "creatureai.h"

namespace engine
{

CreatureAI::CreatureAI(double maxAge, double maxVelocity, double maxAlt, const Creature::CreatureState &state) :
    m_maxAge(maxAge),
    m_maxVelocity(maxVelocity),
    m_maxAlt(maxAlt),
    m_state(state),
    m_curTime(-1.)
{

}

CreatureAI::~CreatureAI()
{

}

void CreatureAI::setState(const Creature::CreatureState &state)
{
    m_state = state;
}

Creature::CreatureState CreatureAI::getState() const
{
    return m_state;
}

}
