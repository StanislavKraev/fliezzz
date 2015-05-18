#include <QLine>

#include "trianglespotpart.h"

TriangleSpotPart::TriangleSpotPart(const QPointF &a, const QPointF &b, const QPointF &c, LandingSpot *parent):
    m_a(a), m_b(b), m_c(c), m_parent(parent)
{

}

TriangleSpotPart::~TriangleSpotPart()
{

}

QPointF TriangleSpotPart::getCenter() const
{
    return QPointF(m_a.x() + m_b.x() + m_c.x(), m_a.y() + m_b.y() + m_c.y()) / 3.0;
}

QRectF TriangleSpotPart::getBBox() const
{
    double minX = std::min(std::min(m_a.x(), m_b.x()), m_c.x());
    double minY = std::min(std::min(m_a.y(), m_b.y()), m_c.y());

    double maxX = std::max(std::max(m_a.x(), m_b.x()), m_c.x());
    double maxY = std::max(std::max(m_a.y(), m_b.y()), m_c.y());
    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

QList<SpotPart*> TriangleSpotPart::split() const
{
    QList<SpotPart*> parts;
    double d1 = QLineF(m_a, m_b).length();
    double d2 = QLineF(m_a, m_c).length();
    double d3 = QLineF(m_b, m_c).length();

    if (d1 > d2)
    {
        if (d3 > d1)
        {
            parts.append(new TriangleSpotPart(m_a, m_b, QPointF((m_b.x() + m_c.x()) / 2., (m_b.y() + m_c.y()) / 2.), m_parent));
            parts.append(new TriangleSpotPart(m_a, m_c, QPointF((m_b.x() + m_c.x()) / 2., (m_b.y() + m_c.y()) / 2.), m_parent));
        }
        else
        {
            parts.append(new TriangleSpotPart(m_a, m_c, QPointF((m_b.x() + m_a.x()) / 2., (m_b.y() + m_a.y()) / 2.), m_parent));
            parts.append(new TriangleSpotPart(m_b, m_c, QPointF((m_b.x() + m_a.x()) / 2., (m_b.y() + m_a.y()) / 2.), m_parent));
        }
    }
    else
    {
        if (d3 > d2)
        {
            parts.append(new TriangleSpotPart(m_a, m_b, QPointF((m_b.x() + m_c.x()) / 2., (m_b.y() + m_c.y()) / 2.), m_parent));
            parts.append(new TriangleSpotPart(m_a, m_c, QPointF((m_b.x() + m_c.x()) / 2., (m_b.y() + m_c.y()) / 2.), m_parent));
        }
        else
        {
            parts.append(new TriangleSpotPart(m_a, m_b, QPointF((m_a.x() + m_c.x()) / 2., (m_a.y() + m_c.y()) / 2.), m_parent));
            parts.append(new TriangleSpotPart(m_b, m_c, QPointF((m_a.x() + m_c.x()) / 2., (m_a.y() + m_c.y()) / 2.), m_parent));
        }
    }
    return parts;
}

double TriangleSpotPart::area() const
{
    double dArea = ((m_b.x() - m_a.x()) * (m_c.y() - m_a.y()) - (m_c.x() - m_a.x()) * (m_b.y() - m_a.y())) / 2.0;
    return abs(dArea);
}

LandingSpot *TriangleSpotPart::parentSpot() const
{
    return m_parent;
}

bool TriangleSpotPart::contains(const QPointF &pt) const
{
    auto sign = [] (const QPointF &p1, const QPointF &p2, const QPointF &p3) -> bool
    {
        return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
    };
    bool b1, b2, b3;

    b1 = sign(pt, m_a, m_b) < 0.0f;
    b2 = sign(pt, m_b, m_c) < 0.0f;
    b3 = sign(pt, m_c, m_a) < 0.0f;

    return ((b1 == b2) && (b2 == b3));
}
