#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QMap>
#include <QUuid>
#include <QGraphicsView>

namespace ui
{

class GraphicsItem;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = 0);
    virtual ~GraphicsView();
public:
    void initGraphics(unsigned short fieldSize, unsigned short pointCapacity);
    GraphicsItem *addItem(const QUuid &uid, GraphicsItem *item);
    GraphicsItem *getItem(const QUuid &uid) const;
    unsigned short getFieldSize() const;
    unsigned short getPointCapacity() const;
protected slots:
    virtual void mousePressEvent(QMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QMouseEvent * mouseEvent);
signals:
    void cellClicked(const QPoint &pt);
private:
    void drawGrid();
    void draw();
private:
    unsigned short m_fieldSize;
    unsigned short m_pointCapacity;
    QMap<QUuid, GraphicsItem*> m_items;
};

}

#endif // GRAPHICSVIEW_H
