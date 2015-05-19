#ifndef FLYAI_H
#define FLYAI_H

#include <QObject>

#include "engine/creatureai.h"
#include "engine/movedirection.h"

class QStateMachine;
class QState;
class QFinalState;

namespace engine
{

class IGameDataProvider;
class IMovesHolder;

class FlyAI: public QObject, public CreatureAI
{
    Q_OBJECT
public:
    const double MinThinkElaps = 0.7;
public:
    FlyAI(double maxAge, double maxVelocity, double maxAlt, double maxThinkTime, const Creature::CreatureState &state,
          IGameDataProvider *gameDataProvider, IMovesHolder *movesHolder);
    virtual ~FlyAI();
public:
    bool isMoving() const;
    void stop();
    int getSimpleState() const;
public:
    virtual void init();
public slots:
    void advance(double dt);
private slots:
    void advanceThinking();
    void advanceFlying();
    void advanceLanding();
    void advanceFalling();

    void onFlyingEnter();
    void onFallingEnter();
    void onDeadEnter();
    void onThinkingEnter();
    void onLandingEnter();
private:
    void changeRoute();
    double angleFromDirection(MoveDirection dir) const;
signals:
    void maxAgeReached();
    void maxFlyingDistanceReached();
    void almostArrived();
    void landed();
    void thinkTimeout();
    void advanceSignal();
    void stopped();
private:
    QStateMachine *m_fsm;
    QState* m_thinkingState;
    QState* m_flyingState;
    QState* m_landingState;
    QState* m_fallingState;
    QFinalState* m_deadState;
    QState* m_curState;
    double m_flyingDuration;
    IGameDataProvider *m_gameDataProvider;
    double m_thinkingTime;
    double m_maxThinkTime;
    MoveDirection m_choosenMove;

    QPoint m_targetSpot;
    QPointF m_targetSpotPart;
    QPointF m_takeOffPt;

    double m_dt;
    IMovesHolder *m_movesHolder;
};

}

#endif // FLYAI_H
