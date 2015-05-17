#include "engine/flyai.h"

#include "fly.h"

Fly::Fly(QObject *parent, const QPoint &startPoint, const QPointF &startPointF,
         double maxAge, double maxVelocity, double maxAlt, double maxThinkTime, IGameDataProvider *gameDataProvider):
    Creature(parent, startPoint, startPointF, maxAge, maxVelocity, maxAlt,
             new FlyAI(maxAge, maxVelocity, maxAlt, maxThinkTime,
                       Creature::CreatureState(startPoint, startPointF, 0, 0, 0, 0, 0), gameDataProvider))
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

Fly::~Fly()
{

}

QRectF Fly::getBBox() const
{
    return QRectF(); // TODO
}

int Fly::getState() const
{
    return 0; // TODO
}

QString Fly::getType() const
{
    return ""; // TODO
}
