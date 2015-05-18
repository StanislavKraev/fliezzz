#include <QDebug>

#include "engine/creatureai.h"

#include "creature.h"

Creature::Creature(const QPoint &startPoint, const QPointF &startPointF,
                   double maxAge, double maxVelocity, double maxAlt):
    m_state(startPoint, startPointF, 0., 0., 0., 0., 0.),
    m_maxVel(maxVelocity), m_maxAlt(maxAlt), m_maxAge(maxAge), m_uid(QUuid::createUuid()),
    m_shouldStop(false), m_time(0.)
{
}

Creature::~Creature()
{
}

//void Creature::advance(double time)
//{
//    m_time = time;
//    m_advanceMutex.unlock();
//}

void Creature::kill()
{
    m_shouldStop = true;
}

QUuid Creature::getUid() const
{
    return m_uid;
}

QPointF Creature::getPosition() const
{
    return m_state.m_transPos;
}

Creature::CreatureState::CreatureState() :
    m_angle(0.), m_vel(0.), m_alt(0.), m_age(0.), m_mileage(0.)
{
}

Creature::CreatureState::CreatureState(QPoint pos, QPointF transPos,
                                       double angle, double vel, double alt,
                                       double age, double mileage) :
    m_pos(pos), m_transPos(transPos), m_angle(angle), m_vel(vel), m_alt(alt), m_age(age), m_mileage(mileage)
{
}

Creature::CreatureState& Creature::CreatureState::operator=(const Creature::CreatureState &right)
{
    if (&right == this) {
        return *this;
    }
    m_pos = right.m_pos;
    m_transPos = right.m_transPos;
    m_angle = right.m_angle;
    m_vel = right.m_vel;
    m_alt = right.m_alt;
    m_age = right.m_age;
    m_mileage = right.m_mileage;
    return *this;
}
