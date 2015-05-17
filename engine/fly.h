#ifndef FLY_H
#define FLY_H

#include <QPoint>
#include <QPointF>
#include <QThread>

#include "engine/creature.h"

class FlyAI;

class IGameDataProvider;

class Fly: public QThread, public Creature
{
    Q_OBJECT
public:
    Fly(IGameDataProvider *gameDataProvider, const QPoint &startPoint, const QPointF &startPointF,
        double maxAge, double maxVelocity, double maxAlt, double maxThinkTime);
    virtual ~Fly();
    virtual QRectF getBBox() const;
    virtual int getState() const;
    virtual QString getType() const;
    void run();
    virtual void advance(double time);
signals:
    void advanceAI(double);
private:
    FlyAI *m_ai;
    double m_maxThinkTime;
    IGameDataProvider *m_gameDataProvider;
};

#endif // FLY_H
