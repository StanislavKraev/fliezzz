#ifndef FLY_H
#define FLY_H

#include <QPoint>
#include <QPointF>

#include "engine/creature.h"

class IGameDataProvider;

class Fly: public Creature
{
public:
    Fly(IGameDataProvider *gameDataProvider, const QPoint &startPoint, const QPointF &startPointF,
        double maxAge, double maxVelocity, double maxAlt, double maxThinkTime);
    virtual ~Fly();
    virtual QRectF getBBox() const;
    virtual int getState() const;
    virtual QString getType() const;
};

#endif // FLY_H
