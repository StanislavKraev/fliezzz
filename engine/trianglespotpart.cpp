#include <QDebug>
#include <QLine>

#include "trianglespotpart.h"

TriangleSpotPart::TriangleSpotPart(const QPointF &a, const QPointF &b, const QPointF &c, LandingSpot *parent):
    m_a(a), m_b(b), m_c(c), m_parent(parent)
{
    m_poly << m_a << m_b << m_c;
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
    return m_poly.boundingRect();
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
    return m_poly.containsPoint(pt, Qt::OddEvenFill);
}

void TriangleSpotPart::tostring() const
{
    qDebug() << "tri spot part: " << m_a << " " << m_b << " " << m_c;
}
