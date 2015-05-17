#ifndef CREATURE_H
#define CREATURE_H

#include <QMutex>
#include <QRect>
#include <QThread>
#include <QUuid>

class CreatureAI;

class Creature: public QThread // todo: aggregate, do not inherit
{
    Q_OBJECT
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
    Creature(QObject *parent, const QPoint &startPoint, const QPointF &startPointF,
             double maxAge, double maxVelocity, double maxAlt, CreatureAI *ai);
    virtual ~Creature();
public:
    virtual QRectF getBBox() const = 0;
    virtual int getState() const = 0;
    virtual QString getType() const = 0;

public:
    QUuid getUid() const;
    QPointF getPosition() const;
    void advance();
    void run();
    void kill();
protected:
    CreatureState m_state;
    double m_maxVel;
    double m_maxAlt;
    double m_maxAge;
    QUuid m_uid;
    CreatureAI *m_ai;
    QMutex m_advanceMutex;
    bool m_shouldStop;
};

#endif // CREATURE_H
