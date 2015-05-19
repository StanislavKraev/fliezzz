#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <QUuid>
#include <QPoint>

class QGraphicsItem;

namespace ui
{

class GraphicsItem
{
public:
    GraphicsItem();
    virtual ~GraphicsItem();
public:
    virtual QUuid getUid() const = 0;
    virtual QGraphicsItem *getQGraphicsItem() = 0;
    virtual void update(const QPoint &pos, double angle) = 0;
};

}

#endif // GRAPHICSITEM_H
