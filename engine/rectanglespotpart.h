#ifndef RECTANGLESPOTPART_H
#define RECTANGLESPOTPART_H

#include "spotpart.h"

class RectangleSpotPart: public SpotPart
{
public:
    RectangleSpotPart(const QRectF &rect, LandingSpot *parent);
    virtual ~RectangleSpotPart();
public:
    virtual QPointF getCenter() const;
    virtual QRectF getBBox() const;
    virtual QList<SpotPart*> split() const;
    virtual double area() const;
    virtual LandingSpot *parentSpot() const;
    virtual bool contains(const QPointF &pt) const;
private:
    QRectF m_rect;
    LandingSpot *m_parent;
};

#endif // RECTANGLESPOTPART_H
