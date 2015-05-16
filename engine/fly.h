#ifndef FLY_H
#define FLY_H

#include <QPoint>
#include <QPointF>

#include "engine/creature.h"

class CreatureAI;

class Fly: public Creature
{
public:
    Fly(const QPoint &startPoint, const QPointF &startPointF, double maxAge, double maxVelocity, double maxAlt, CreatureAI *ai);
    virtual ~Fly();
    virtual QRectF getBBox() const;
private:

    CreatureAI *m_ai;
};

#endif // FLY_H
