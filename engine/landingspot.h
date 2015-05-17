#ifndef LANDINGSPOT_H
#define LANDINGSPOT_H

#include <QVector>

class SpotPart;

class LandingSpot
{
public:
    LandingSpot(unsigned int x, unsigned int y, double spotSize, unsigned short spotCapacity);
    virtual ~LandingSpot();
private:
    QVector<SpotPart *> m_landingSpots;
    unsigned int m_x;
    unsigned int m_y;
};

#endif // LANDINGSPOT_H
