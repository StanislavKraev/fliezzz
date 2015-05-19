#ifndef LANDINGSPOT_H
#define LANDINGSPOT_H

#include <QRect>
#include <QVector>

namespace engine
{


class SpotPart;

class LandingSpot
{
public:
    LandingSpot(unsigned int x, unsigned int y, double spotSize, unsigned short spotCapacity);
    virtual ~LandingSpot();
public:
    QRectF getBBox() const;
    unsigned short getPartsCount() const;
    short getPartIndexFromPt(const QPointF &pt) const;
    SpotPart *getPart(unsigned short index) const;
private:
    QVector<SpotPart *> m_landingSpots;
    unsigned int m_x;
    unsigned int m_y;
    QRectF m_bbox;
};

}

#endif // LANDINGSPOT_H
