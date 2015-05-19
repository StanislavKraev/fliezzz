#ifndef FLYGRAPHICSITEM_H
#define FLYGRAPHICSITEM_H

#include <QGraphicsPixmapItem>
#include <QUuid>
#include <QPoint>

#include "graphicsitem.h"

namespace ui
{

class FlyGraphicsItem: public QGraphicsPixmapItem, public GraphicsItem
{
public:
    explicit FlyGraphicsItem(const QUuid &flyUid, double width, double height);
    virtual ~FlyGraphicsItem();
public:
    virtual QUuid getUid() const;
    virtual QGraphicsItem *getQGraphicsItem();
    virtual void update(const QPoint &pos, double angle, int state);
private:
    QUuid m_flyUid;
    double m_width;
    double m_height;
};

}

#endif // FLYGRAPHICSITEM_H
