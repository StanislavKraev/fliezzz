#include "engine/trianglespotpart.h"

#include "rectanglespotpart.h"

RectangleSpotPart::RectangleSpotPart(const QRectF &rect, LandingSpot *parent) :
    m_rect(rect), m_parent(parent)
{
}

RectangleSpotPart::~RectangleSpotPart()
{
}

QPointF RectangleSpotPart::getCenter() const
{
    return m_rect.center();
}

QRectF RectangleSpotPart::getBBox() const
{
    return m_rect;
}

QList<SpotPart*> RectangleSpotPart::split() const
{
    QList<SpotPart*> parts;
    parts.append(new TriangleSpotPart(m_rect.topLeft(), m_rect.topRight(), m_rect.bottomLeft(), m_parent));
    parts.append(new TriangleSpotPart(m_rect.topRight(), m_rect.bottomRight(), m_rect.bottomLeft(), m_parent));
    return parts;
}

double RectangleSpotPart::area() const
{
    return m_rect.width() * m_rect.height();
}

LandingSpot *RectangleSpotPart::parentSpot() const
{
    return m_parent;
}
