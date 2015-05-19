#ifndef CREATURE_H
#define CREATURE_H

#include <QRect>
#include <QUuid>

namespace engine
{

class Creature
{   
public:
    struct CreatureState{
        CreatureState();
        CreatureState(QPoint pos, QPointF transPos, double angle, double vel, double alt, double age, double mileage);
        CreatureState& operator=(const CreatureState &right);
    public:
        QPoint m_pos;
        QPointF m_transPos;
        double m_angle;
        double m_vel;
        double m_alt;
        double m_age;
        double m_mileage;
    };

public:
    Creature(const QPoint &startPoint, const QPointF &startPointF,
             double maxAge, double maxVelocity, double maxAlt);
    virtual ~Creature();
public:
    virtual QRectF getBBox() const = 0;
    virtual int getState() const = 0;
    virtual QString getType() const = 0;
    virtual bool isMoving() const = 0;
    virtual void advance(double dt) = 0;
    virtual double getMaxThinkTime() const = 0;

    virtual void stopAI() = 0;

public:
    QUuid getUid() const;
    QPointF getPosition() const;
    void kill();
    double getAngle() const;
protected:
    CreatureState m_state;
    double m_maxVel;
    double m_maxAlt;
    double m_maxAge;
    QUuid m_uid;
    bool m_shouldStop;
    double m_time;
};

}

#endif // CREATURE_H
