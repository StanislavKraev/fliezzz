#include <QDebug>
#include <QGraphicsOpacityEffect>

#include "flygraphicsitem.h"

namespace ui
{

FlyGraphicsItem::FlyGraphicsItem(const QUuid &flyUid, double width, double height) :
    m_flyUid(flyUid),
    m_width(width),
    m_height(height)
{
    setVisible(false);
    setPixmap(QPixmap(QString(":/ui/fly.png")));
    setTransformationMode(Qt::SmoothTransformation);
}

FlyGraphicsItem::~FlyGraphicsItem()
{
}

QUuid FlyGraphicsItem::getUid() const
{
    return m_flyUid;
}

QGraphicsItem *FlyGraphicsItem::getQGraphicsItem()
{
    return this;
}

void FlyGraphicsItem::update(const QPoint &pos, double angle, int state)
{
    setVisible(true);
    if (state == 100)
    {
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
        effect->setOpacity(0.3);
        setGraphicsEffect(effect);
        return;
    }
    double scale = 1.0 / 60. * m_height;
    resetTransform();
    QRectF brc=boundingRect();
    setTransform(QTransform::fromTranslate(-brc.width()/2.0, -brc.height()/2.0) *
                 QTransform::fromScale(scale, scale).rotate(angle) *
                 QTransform::fromTranslate(pos.x(), pos.y()), true);
}

}
