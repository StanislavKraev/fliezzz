#ifndef TRIANGLESPOTPART_H
#define TRIANGLESPOTPART_H

#include <QPolygonF>

#include <engine/spotpart.h>

class TriangleSpotPart: public SpotPart
{
public:
    TriangleSpotPart(const QPointF &a, const QPointF &b, const QPointF &c, LandingSpot *parent);
    virtual ~TriangleSpotPart();
public:
    virtual QPointF getCenter() const;
    virtual QRectF getBBox() const;
    virtual QList<SpotPart*> split() const;
    virtual double area() const;
    virtual LandingSpot *parentSpot() const;
    virtual bool contains(const QPointF &pt) const;

    virtual void tostring() const;
private:
    QPointF m_a;
    QPointF m_b;
    QPointF m_c;
    LandingSpot *m_parent;
    QPolygonF m_poly;
};

#endif // TRIANGLESPOTPART_H
