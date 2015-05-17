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
        emit(thinkTimeout());
    }
}

void FlyAI::advanceFlying(double dt)
{
    m_flyingDuration += dt;
}

void FlyAI::advanceLanding(double dt)
{
}

void FlyAI::advanceFalling(double dt)
{
}

void FlyAI::onFlyingEnter()
{
    qDebug() << "Flying enter";
    m_flyingDuration = 0; // todo: shouldn't be reset on continuos flights (when landing was not free)

}

void FlyAI::onFallingEnter()
{
    qDebug() << "Falling enter";
}

void FlyAI::onDeadEnter()
{
    qDebug() << "Dead enter";
}

void FlyAI::onThinkingEnter()
{
    qDebug() << "Thinking enter";
}

void FlyAI::onLandingEnter()
{
    qDebug() << "Landing enter";
}
