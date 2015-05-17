#include <QState>
#include <QFinalState>
#include <QSignalTransition>
#include <QStateMachine>
#include <QDebug>

#include "engine/igamedataprovider.h"

#include "flyai.h"

double frand(double from = 0., double to = 1.)
{
    double f = (double)rand() / RAND_MAX;
    return from + f * (to - from);
}

FlyAI::FlyAI(double maxAge, double maxVelocity, double maxAlt, double maxThinkTime, const Creature::CreatureState &state,
             IGameDataProvider *gameDataProvider):
    CreatureAI(maxAge, maxVelocity, maxAlt, state),
    m_flyingDuration(0.),
    m_gameDataProvider(gameDataProvider),
    m_thinkingTime(0.),
    m_maxThinkTime(maxThinkTime),
    m_choosenMove(MoveDirection::MdUnknown),
    m_targetSpot(0, 0),
    m_targetSpotPart(0., 0.)
{
    m_fsm = new QStateMachine();

    QState *thinkingState = new QState();
    m_fsm->addState(thinkingState);

    QState *flyingState = new QState();
    m_fsm->addState(flyingState);

    QState *landingState = new QState();
    m_fsm->addState(landingState);

    QState *fallingState = new QState();
    m_fsm->addState(fallingState);

    QFinalState *deadState = new QFinalState();
    m_fsm->addState(deadState);

    thinkingState->addTransition(this, SIGNAL(thinkTimeout()), flyingState);
    thinkingState->addTransition(this, SIGNAL(maxAgeReached()), deadState);
    QObject::connect(thinkingState, SIGNAL(entered()), this, SLOT(onThinkingEnter()));

    flyingState->addTransition(this, SIGNAL(almostArrived()), landingState);
    flyingState->addTransition(this, SIGNAL(maxAgeReached()), fallingState);
    flyingState->addTransition(this, SIGNAL(maxFlyingDistanceReached()), fallingState);
    QObject::connect(flyingState, SIGNAL(entered()), this, SLOT(onFlyingEnter()));

    landingState->addTransition(this, SIGNAL(maxAgeReached()), fallingState);
    landingState->addTransition(this, SIGNAL(maxFlyingDistanceReached()), fallingState);
    landingState->addTransition(this, SIGNAL(thinkTimeout()), flyingState);
    landingState->addTransition(this, SIGNAL(landed()), thinkingState);
    QObject::connect(landingState, SIGNAL(entered()), this, SLOT(onLandingEnter()));

    fallingState->addTransition(this, SIGNAL(stopped()), deadState);
    QObject::connect(fallingState, SIGNAL(entered()), this, SLOT(onFallingEnter()));

    QObject::connect(deadState, SIGNAL(entered()), this, SLOT(onDeadEnter()));

    m_fsm->setInitialState(thinkingState);
    QSignalTransition *trans = new QSignalTransition(this, SIGNAL(advanceSignal(double)));
    thinkingState->addTransition(trans);
    QObject::connect(trans, SIGNAL(triggered()), this, SLOT(advanceThinking()));

    QSignalTransition *transFlying = new QSignalTransition(this, SIGNAL(advanceSignal(double)));
    flyingState->addTransition(transFlying);
    QObject::connect(transFlying, SIGNAL(triggered()), this, SLOT(advanceFlying()));

    QSignalTransition *transLanding = new QSignalTransition(this, SIGNAL(advanceSignal(double)));
    landingState->addTransition(transLanding);
    QObject::connect(transLanding, SIGNAL(triggered()), this, SLOT(advanceLanding()));

    QSignalTransition *transFalling = new QSignalTransition(this, SIGNAL(advanceSignal(double)));
    fallingState->addTransition(transFalling);
    QObject::connect(transFalling, SIGNAL(triggered()), this, SLOT(advanceFalling()));
}

FlyAI::~FlyAI()
{
    m_fsm->deleteLater();
    m_fsm = nullptr;
}

void FlyAI::advance(double time, Creature::CreatureState &newState)
{
    if (m_curTime < 0.)
    {
        m_curTime = time;
        return;
    }

    double dt = time - m_curTime;
    if (dt < 0.)
    {
        qWarning() << "Invalid time";
        return;
    }

    m_curTime = time;
    emit advanceSignal(dt);
    newState=m_state;
}

void FlyAI::advanceThinking(double dt)
{
    m_state.m_age += dt;
    if (m_state.m_age > m_maxAge)
    {
        emit(maxAgeReached());
        return;
    }

    QSet<MoveDirection> moveDirections;
    m_gameDataProvider->getMovesFromPoint(m_state.m_pos, moveDirections);
    if (moveDirections.empty())
    {
        return;
    }

    m_thinkingTime += dt;
    bool timeIsElapsed = m_thinkingTime >= m_maxThinkTime * FlyAI::MinThinkElaps;
    bool maxTimeIsElapsed = m_thinkingTime >= m_maxThinkTime;

    if (timeIsElapsed)
    {
        if (frand() > 0.5)
        {
            if (m_choosenMove != MoveDirection::MdUnknown && moveDirections.contains(m_choosenMove))
            {
                m_targetSpot = m_gameDataProvider->getPointByDirection(m_state.m_pos, m_choosenMove);
            }
            else
            {
                m_targetSpot = m_gameDataProvider->getPointByDirection(m_state.m_pos, *(moveDirections.begin()));
            }
            m_gameDataProvider->getFreeLandingPoint(m_targetSpot);
            emit(thinkTimeout());
        }
    }

    if (maxTimeIsElapsed)
    {
        if (m_choosenMove != MoveDirection::MdUnknown && moveDirections.contains(m_choosenMove))
        {
            m_targetSpot = m_gameDataProvider->getPointByDirection(m_state.m_pos, m_choosenMove);
        }
        else
        {
            m_targetSpot = m_gameDataProvider->getPointByDirection(m_state.m_pos, *(moveDirections.begin()));
        }
        m_gameDataProvider->getFreeLandingPoint(m_targetSpot);
        emit(thinkTimeout());
    }
}

void FlyAI::advanceFlying(double dt)
{
    m_flyingDuration += dt;

    double s = m_flyingDuration * m_state.m_vel;
    double dx = (m_targetSpotPart.x() - m_takeOffPt.x());
    double dy = (m_targetSpotPart.y() - m_takeOffPt.y());
    double c = sqrt(dx * dx + dy * dy);
    double maxS = c * m_state.m_vel;

    if (s >= maxS)
    {
        s = maxS;
    }

    double cosA = dx / c;
    double sinA = dy / c;

    m_state.m_transPos = QPointF(s * cosA, s * sinA);

    m_state.m_age += dt;
    if (m_state.m_age > m_maxAge)
    {
        emit(maxAgeReached());
        return;
    }

    if (s >= maxS * 0.7)
    {
        emit almostArrived();
    }
}

void FlyAI::advanceLanding(double dt)
{
    m_flyingDuration += dt;

    double s = m_flyingDuration * m_state.m_vel;
    double dx = (m_targetSpotPart.x() - m_takeOffPt.x());
    double dy = (m_targetSpotPart.y() - m_takeOffPt.y());
    double c = sqrt(dx * dx + dy * dy);
    double maxS = c * m_state.m_vel;

    if (s >= maxS)
    {
        m_state.m_transPos = m_targetSpotPart;
        m_state.m_pos = m_targetSpot;
        emit landed();
        return;
    }

    double cosA = dx / c;
    double sinA = dy / c;

    m_state.m_transPos = QPointF(s * cosA, s * sinA);

    m_state.m_age += dt;
    if (m_state.m_age > m_maxAge)
    {
        emit maxAgeReached();
        return;
    }
}

void FlyAI::advanceFalling(double dt)
{
    m_state.m_vel -= m_maxVelocity * 0.01 * dt;
    if (m_state.m_vel < 0.)
    {
        m_state.m_vel = 0;
    }
    m_state.m_age += dt;
    m_flyingDuration += dt;

    double cosA = cos(m_state.m_angle);
    double sinA = sin(m_state.m_angle);

    double s = m_state.m_vel * dt;
    m_state.m_transPos = QPointF(s * cosA, s * sinA); // todo: ensure not to exit from the game field

    if (m_state.m_vel <= 0.)
    {
        emit stopped();
    }
}

void FlyAI::onFlyingEnter()
{
    qDebug() << "Flying enter";
    m_flyingDuration = 0; // todo: shouldn't be reset on continuos flights (when landing was not free)
    m_state.m_vel = m_maxVelocity * frand(0.8, 1.);
    m_takeOffPt = m_state.m_transPos;

    double dx = (m_targetSpotPart.x() - m_takeOffPt.x());
    double dy = (m_targetSpotPart.y() - m_takeOffPt.y());
    double c = sqrt(dx * dx + dy * dy);
    double cosA = dx / c;

    m_state.m_angle = acos(cosA);
}

void FlyAI::onFallingEnter()
{
    qDebug() << "Falling enter";
}

void FlyAI::onDeadEnter()
{
    qDebug() << "Dead enter";
    m_state.m_vel = 0;
    m_flyingDuration = 0;
    m_choosenMove = MoveDirection::MdUnknown;
}

void FlyAI::onThinkingEnter()
{
    qDebug() << "Thinking enter";
    m_state.m_vel = 0;
    m_flyingDuration = 0;
    m_choosenMove = MoveDirection::MdUnknown;
    m_thinkingTime = 0;
}

void FlyAI::onLandingEnter()
{
    qDebug() << "Landing enter";
}
