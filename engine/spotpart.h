#ifndef SPOTPART_H
#define SPOTPART_H

#include <QPointF>
#include <QRectF>
#include <QList>

namespace engine
{

class LandingSpot;

class SpotPart
{
public:
    SpotPart();
    virtual ~SpotPart();
private:
    SpotPart(const SpotPart &);
public:
    virtual QPointF getCenter() const = 0;
    virtual QRectF getBBox() const = 0;
    virtual QList<SpotPart*> split() const = 0;
    virtual double area() const = 0;
    virtual LandingSpot *parentSpot() const = 0;
    virtual bool contains(const QPointF &pt) const = 0;

    virtual void tostring() const = 0;
};

}

#endif // SPOTPART_H
