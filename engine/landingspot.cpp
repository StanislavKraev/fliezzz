#include <QDebug>
#include "rectanglespotpart.h"

#include "landingspot.h"

LandingSpot::LandingSpot(unsigned int x, unsigned int y, double spotSize, unsigned short spotCapacity) :
    m_x(x), m_y(y)
{
    Q_ASSERT(spotCapacity > 0);
    m_landingSpots.resize(spotCapacity);
    double rx = double(x) * spotSize;
    double ry = double(y) * spotSize;
    SpotPart *part = new RectangleSpotPart(QRectF(QPointF(rx, ry), QPointF(rx + spotSize, ry + spotSize)), this);

    if (spotCapacity == 1)
    {
        m_landingSpots[0] = part;
    }

    QList<SpotPart *> parts = part->split();
    delete part;

    m_landingSpots[0] = parts.at(0);
    m_landingSpots[1] = parts.at(1);
    if (spotCapacity == 2)
    {
        return;
    }

    while (parts.count() < spotCapacity)
    {
        SpotPart *biggestPart = parts.takeFirst();
        parts.append(biggestPart->split());
        delete biggestPart;
    }
    unsigned int i = 0;
    for (auto nextPart: parts)
    {
        m_landingSpots[i] = nextPart;
        i++;
    }
}

LandingSpot::~LandingSpot()
{
    for (unsigned int i = 0; i < m_landingSpots.count(); ++i)
    {
        delete m_landingSpots[i];
    }
}

