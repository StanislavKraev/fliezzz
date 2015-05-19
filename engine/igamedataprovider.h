#ifndef IGAMEDATAPROVIDER_H
#define IGAMEDATAPROVIDER_H

#include <memory>

#include <QPoint>
#include <QPointF>
#include <QUuid>

#include "engine/movedirection.h"

namespace engine
{


class IGameDataProvider
{
public:
    virtual unsigned short getSize() const = 0;
    virtual unsigned short getPointCapacity() const = 0;
    virtual void getMovesFromPoint(const QPoint &pt, QSet<MoveDirection> &moveDirectionSet) const = 0;
    virtual std::shared_ptr<QPointF> getFreeLandingPoint(const QPoint &pt) const = 0;
    virtual bool isLandingPointFree(const QPointF &pt) const = 0;
    virtual QUuid getCreatureAt(const QPointF &pt) const = 0;
    virtual QPoint getPointByDirection(const QPoint &pt, MoveDirection moveDirection) const = 0;
protected:
    IGameDataProvider();
    ~IGameDataProvider();
private:
    IGameDataProvider(const IGameDataProvider&);
};

}

#endif // IGAMEDATAPROVIDER_H
