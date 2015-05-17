#ifndef IGAMEDATAPROVIDER_H
#define IGAMEDATAPROVIDER_H

#include <QPoint>
#include <QPointF>
#include <QUuid>

#include "engine/movedirection.h"

class IGameDataProvider
{
public:
    virtual unsigned short getSize() const = 0;
    virtual unsigned short getPointCapacity() const = 0;
    virtual void getMovesFromPoint(const QPoint &pt, QSet<MoveDirection> &moveDirectionSet) const = 0;
    virtual QPointF getFreeLandingPoint(const QPoint &pt) const = 0;
    virtual bool isLandingPointFree(const QPointF &pt) const = 0;
    virtual QUuid getCreatureAt(const QPointF &pt) const = 0;
protected:
    IGameDataProvider();
    ~IGameDataProvider();
private:
    IGameDataProvider(const IGameDataProvider&);
};

#endif // IGAMEDATAPROVIDER_H
