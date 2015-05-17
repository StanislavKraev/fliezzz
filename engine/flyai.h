#ifndef FLYAI_H
#define FLYAI_H

#include <QObject>

#include "engine/creatureai.h"
#include "engine/movedirection.h"

class QStateMachine;
class IGameDataProvider;

class FlyAI: public QObject, public CreatureAI
{
    Q_OBJECT
public:
    const double MinThinkElaps = 0.7;
public:
    FlyAI(double maxAge, double maxVelocity, double maxAlt, double maxThinkTime, const Creature::CreatureState &state,
          IGameDataProvider *gameDataProvider);
    virtual ~FlyAI();
public:
    virtual void advance(double time, Creature::CreatureState &newState);
private slots:
    void advanceThinking(double dt);
    void advanceFlying(double dt);
    void advanceLanding(double dt);
    void advanceFalling(double dt);

    void onFlyingEnter();
    void onFallingEnter();
    void onDeadEnter();
    void onThinkingEnter();
    void onLandingEnter();
signals:
    void maxAgeReached();
    void maxFlyingDistanceReached();
    void almostArrived();
    void landed();
    void thinkTimeout();
    void advanceSignal(double dt);
    void stopped();
private:
    QStateMachine *m_fsm;
    double m_flyingDuration;
    IGameDataProvider *m_gameDataProvider;
    double m_thinkingTime;
    double m_maxThinkTime;
    MoveDirection m_choosenMove;

    QPoint m_targetSpot;
    QPointF m_targetSpotPart;
};

#endif // FLYAI_H
