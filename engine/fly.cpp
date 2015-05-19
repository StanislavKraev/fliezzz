#include <QDebug>

#include "engine/flyai.h"

#include "fly.h"

namespace engine
{

Fly::Fly(IGameDataProvider *gameDataProvider, const QPoint &startPoint, const QPointF &startPointF,
         double maxAge, double maxVelocity, double maxAlt, double maxThinkTime):
    Creature(startPoint, startPointF, maxAge, maxVelocity, maxAlt),
    m_ai(nullptr),
    m_maxThinkTime(maxThinkTime),
    m_gameDataProvider(gameDataProvider)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

Fly::~Fly()
{
    if (m_ai)
    {
        m_ai->deleteLater();
        m_ai = nullptr;
    }
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
    return "fly";
}

void Fly::run()
{
    m_ai = new FlyAI(m_maxAge, m_maxVel, m_maxAlt, m_maxThinkTime, m_state, m_gameDataProvider);
    m_ai->init();

    connect(this, SIGNAL(advanceAI(double)),
            m_ai, SLOT(advance(double)), Qt::QueuedConnection);
    exec();
}

void Fly::advance(double dt)
{
    emit advanceAI(dt);
    m_state = m_ai->getState();
}

bool Fly::isMoving() const
{
    if (!m_ai)
    {
        return false;
    }
    return m_ai->isMoving();
}

void Fly::stopAI()
{
    if (m_ai)
    {
        m_ai->stop();
    }
    exit();
    wait();
}

}
