#include <QCoreApplication>
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

namespace engine
{

FlyAI::FlyAI(double maxAge, double maxVelocity, double maxAlt, double maxThinkTime, const Creature::CreatureState &state,
             IGameDataProvider *gameDataProvider):
    CreatureAI(maxAge, maxVelocity, maxAlt, state),
    m_fsm(nullptr),
    m_thinkingState(nullptr),
    m_flyingState(nullptr),
    m_landingState(nullptr),
    m_fallingState(nullptr),
    m_deadState(nullptr),
    m_curState(nullptr),
    m_flyingDuration(0.),
    m_gameDataProvider(gameDataProvider),
    m_thinkingTime(0.),
    m_maxThinkTime(maxThinkTime),
    m_choosenMove(MoveDirection::MdUnknown),
    m_targetSpot(0, 0),
    m_targetSpotPart(0., 0.),
    m_dt(0.)
{
}

void FlyAI::init()
{
    Q_ASSERT(m_fsm == nullptr);

    m_fsm = new QStateMachine();

    m_thinkingState = new QState(m_fsm);
    m_flyingState = new QState(m_fsm);
    m_landingState = new QState(m_fsm);
    m_fallingState = new QState(m_fsm);
    m_deadState = new QFinalState(m_fsm);

    m_thinkingState->addTransition(this, SIGNAL(thinkTimeout()), m_flyingState);
    m_thinkingState->addTransition(this, SIGNAL(maxAgeReached()), m_deadState);
    QObject::connect(m_thinkingState, SIGNAL(entered()), this, SLOT(onThinkingEnter()));

    m_flyingState->addTransition(this, SIGNAL(almostArrived()), m_landingState);
    m_flyingState->addTransition(this, SIGNAL(maxAgeReached()), m_fallingState);
    m_flyingState->addTransition(this, SIGNAL(maxFlyingDistanceReached()), m_fallingState);
    QObject::connect(m_flyingState, SIGNAL(entered()), this, SLOT(onFlyingEnter()));

    m_landingState->addTransition(this, SIGNAL(maxAgeReached()), m_fallingState);
    m_landingState->addTransition(this, SIGNAL(maxFlyingDistanceReached()), m_fallingState);
    m_landingState->addTransition(this, SIGNAL(thinkTimeout()), m_flyingState);
    m_landingState->addTransition(this, SIGNAL(landed()), m_thinkingState);
    QObject::connect(m_landingState, SIGNAL(entered()), this, SLOT(onLandingEnter()));

    m_fallingState->addTransition(this, SIGNAL(stopped()), m_deadState);
    QObject::connect(m_fallingState, SIGNAL(entered()), this, SLOT(onFallingEnter()));

    QObject::connect(m_deadState, SIGNAL(entered()), this, SLOT(onDeadEnter()));

    QSignalTransition *trans = new QSignalTransition(this, SIGNAL(advanceSignal()));
    m_thinkingState->addTransition(trans);
    QObject::connect(trans, SIGNAL(triggered()), this, SLOT(advanceThinking()));

    QSignalTransition *transFlying = new QSignalTransition(this, SIGNAL(advanceSignal()));
    m_flyingState->addTransition(transFlying);
    QObject::connect(transFlying, SIGNAL(triggered()), this, SLOT(advanceFlying()));

    QSignalTransition *transLanding = new QSignalTransition(this, SIGNAL(advanceSignal()));
    m_landingState->addTransition(transLanding);
    QObject::connect(transLanding, SIGNAL(triggered()), this, SLOT(advanceLanding()));

    QSignalTransition *transFalling = new QSignalTransition(this, SIGNAL(advanceSignal()));
    m_fallingState->addTransition(transFalling);
    QObject::connect(transFalling, SIGNAL(triggered()), this, SLOT(advanceFalling()));

    m_fsm->setInitialState(m_thinkingState);
    m_fsm->start();
    qDebug() << "fly fsm started";
}

FlyAI::~FlyAI()
{
    m_fsm->deleteLater();
    m_fsm = nullptr;
}

void FlyAI::advance(double dt)
{
    m_dt = dt;
    if (dt <= 0)
    {
        return;
    }
    emit advanceSignal();
}

void FlyAI::advanceThinking()
{
    double dt = m_dt;
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
        m_choosenMove = MoveDirection::MdUnknown;
        qDebug() << "No move directions. Waiting";
        return;
    }

    m_thinkingTime += dt;
    bool timeIsElapsed = m_thinkingTime >= m_maxThinkTime * FlyAI::MinThinkElaps;
    bool maxTimeIsElapsed = m_thinkingTime >= m_maxThinkTime;

    if (m_choosenMove == MoveDirection::MdUnknown || !moveDirections.contains(m_choosenMove))
    {
        int choosenIndex = int(frand() * (double)(moveDirections.count()));
        auto dirIt = moveDirections.begin();
        while(choosenIndex > 0)
        {
            dirIt ++;
            --choosenIndex;
        }
        m_choosenMove = *dirIt;
    }

    // todo: set angle

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
                int choosenIndex = int(frand() * (double)(moveDirections.count()));
                auto dirIt = moveDirections.begin();
                while(choosenIndex > 0)
                {
                    dirIt ++;
                    --choosenIndex;
                }
                m_choosenMove = *dirIt;

                m_targetSpot = m_gameDataProvider->getPointByDirection(m_state.m_pos, m_choosenMove);
            }
            std::shared_ptr<QPointF> part = m_gameDataProvider->getFreeLandingPoint(m_targetSpot);
            if (!part)
            {
                qWarning() << "Can't move to choosen direction. Continue thinking";
                return;
            }
            m_targetSpotPart = *part;
            qDebug() << "Direction: " << m_choosenMove << " target spot: " << m_targetSpotPart;
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
        std::shared_ptr<QPointF> part = m_gameDataProvider->getFreeLandingPoint(m_targetSpot);
        if (!part)
        {
            qWarning() << "Can't move to choosen direction. Continue thinking";
            return;
        }
        m_targetSpotPart = *part;
        qDebug() << "Direction: " << m_choosenMove << " target spot: " << m_targetSpotPart;
        emit(thinkTimeout());
    }
    //qDebug() << "Think further. " << m_thinkingTime << " pos:" << m_state.m_transPos;
}

void FlyAI::advanceFlying()
{
    double dt = m_dt;
    m_flyingDuration += dt;

    double s = m_flyingDuration * m_state.m_vel;
    double dx = (m_targetSpotPart.x() - m_takeOffPt.x());
    double dy = (m_targetSpotPart.y() - m_takeOffPt.y());
    double c = sqrt(dx * dx + dy * dy);
    double maxS = c;

    if (s >= maxS)
    {
        s = maxS;
    }

    double cosA = dx / c;
    double sinA = dy / c;

    m_state.m_transPos = m_takeOffPt + QPointF(s * cosA, s * sinA);

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
    //qDebug() << "flying: " << m_state.m_transPos;
}

void FlyAI::advanceLanding()
{
    double dt = m_dt;
    m_flyingDuration += dt;

    double s = m_flyingDuration * m_state.m_vel;
    double dx = (m_targetSpotPart.x() - m_takeOffPt.x());
    double dy = (m_targetSpotPart.y() - m_takeOffPt.y());
    double c = sqrt(dx * dx + dy * dy);
    double maxS = c;

    if (s >= maxS)
    {
        m_state.m_transPos = m_targetSpotPart;
        m_state.m_pos = m_targetSpot;

        if (!m_gameDataProvider->isLandingPointFree(m_targetSpotPart))
        {
            changeRoute();
            return;
        }

        //qDebug() << "landing: " << m_state.m_transPos;
        emit landed();
        return;
    }

    double cosA = dx / c;
    double sinA = dy / c;

    m_state.m_transPos = m_takeOffPt + QPointF(s * cosA, s * sinA);

    //qDebug() << "landing: " << m_state.m_transPos;

    m_state.m_age += dt;
    if (m_state.m_age > m_maxAge)
    {
        emit maxAgeReached();
        return;
    }

    if (!m_gameDataProvider->isLandingPointFree(m_targetSpotPart))
    {
        changeRoute();
    }
}

void FlyAI::advanceFalling()
{
    double dt = m_dt;
    m_state.m_vel -= m_maxVelocity * dt;
    if (m_state.m_vel < 0.)
    {
        m_state.m_vel = 0;
    }
    m_state.m_age += dt;
    m_flyingDuration += dt;

    double cosA = cos(m_state.m_angle);
    double sinA = sin(m_state.m_angle);

    double s = m_state.m_vel * dt;
    m_state.m_transPos = m_takeOffPt + QPointF(s * cosA, s * sinA); // todo: ensure not to exit from the game field

    if (m_state.m_vel <= 0.)
    {
        emit stopped();
    }

    qDebug() << "falling: " << m_state.m_transPos;
}

void FlyAI::onFlyingEnter()
{
    m_curState = m_flyingState;
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
    m_curState = m_fallingState;
    qDebug() << "Falling enter";
}

void FlyAI::onDeadEnter()
{
    m_curState = 0; // hack
    qDebug() << "Dead enter";
    m_state.m_vel = 0;
    m_flyingDuration = 0;
    m_choosenMove = MoveDirection::MdUnknown;
}

void FlyAI::onThinkingEnter()
{
    m_curState = m_thinkingState;
    qDebug() << "Thinking enter";
    m_state.m_vel = 0;
    m_flyingDuration = 0;
    m_choosenMove = MoveDirection::MdUnknown;
    m_thinkingTime = 0;
}

void FlyAI::onLandingEnter()
{
    m_curState = m_landingState;
    qDebug() << "Landing enter";
}

bool FlyAI::isMoving() const
{
    return m_curState == m_flyingState || m_curState == m_landingState || m_curState == m_fallingState;
}

void FlyAI::changeRoute()
{
    QSet<MoveDirection> moveDirections;
    m_gameDataProvider->getMovesFromPoint(m_state.m_pos, moveDirections);
    m_choosenMove = MoveDirection::MdUnknown;
    if (moveDirections.empty())
    {
        qDebug() << "No move directions. Dying";
        emit maxFlyingDistanceReached(); // todo: create correct signal
        return;
    }

    int choosenIndex = int(frand() * (double)(moveDirections.count()));
    auto dirIt = moveDirections.begin();
    while(choosenIndex > 0)
    {
        dirIt ++;
        --choosenIndex;
    }
    m_choosenMove = *dirIt;

    m_targetSpot = m_gameDataProvider->getPointByDirection(m_state.m_pos, m_choosenMove);

    std::shared_ptr<QPointF> part = m_gameDataProvider->getFreeLandingPoint(m_targetSpot);
    if (!part)
    {
        qDebug() << "No moves. Dying";
        emit maxFlyingDistanceReached(); // todo: create correct signal
        return;
    }
    m_targetSpotPart = *part;
    emit thinkTimeout(); // todo: create correct signal
}

}
