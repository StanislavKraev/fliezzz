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
    m_bbox = QRectF(QPointF(rx, ry), QPointF(rx + spotSize, ry + spotSize));
    //qDebug() << "add spot: " << m_bbox;
    SpotPart *part = new RectangleSpotPart(m_bbox, this);

    if (spotCapacity == 1)
    {
        m_landingSpots[0] = part;
    }

    QList<SpotPart *> parts = part->split();
    delete part;
    //qDebug() << "first split 1: " << parts.at(0)->getCenter();
    //qDebug() << "first split 2: " << parts.at(1)->getCenter();

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
        m_landingSpots[i++] = nextPart;
    }

//    for (auto nextPart2: m_landingSpots)
//    {
//        nextPart2->tostring();
//    }
}

LandingSpot::~LandingSpot()
{
    for (unsigned int i = 0; i < m_landingSpots.count(); ++i)
    {
        delete m_landingSpots[i];
    }
}

QRectF LandingSpot::getBBox() const
{
    return m_bbox;
}

unsigned short LandingSpot::getPartsCount() const
{
    return m_landingSpots.count();
}

short LandingSpot::getPartIndexFromPt(const QPointF &pt) const
{
    //qDebug() << "LandingSpot::getPartIndexFromPt " << pt << " inside " << m_bbox;
    unsigned short index = 0;
    for (auto part: m_landingSpots)
    {
        if (part->contains(pt)) {
            //qDebug() << "LandingSpot::getPartIndexFromPt result: " << index;
            return index;
        }
        ++index;
    }
    //qDebug() << "LandingSpot::getPartIndexFromPt result: -1";
    return -1;
}

SpotPart *LandingSpot::getPart(unsigned short index) const
{
    return index >= 0 && index <= m_landingSpots.count() ? m_landingSpots[index] : nullptr;
}
