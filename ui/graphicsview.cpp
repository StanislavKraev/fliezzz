#include <QMouseEvent>
#include <QDebug>

#include "graphicsitem.h"
#include "graphicsview.h"

namespace ui
{

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent), m_fieldSize(0), m_pointCapacity(0)
{

}

GraphicsView::~GraphicsView()
{

}

void GraphicsView::mousePressEvent(QMouseEvent * mouseEvent)
{
    //qDebug() << "pressed";
}

void GraphicsView::mouseReleaseEvent(QMouseEvent * mouseEvent)
{
    QPoint pt = mouseEvent->pos();
    if (m_fieldSize < 1) {
        return;
    }
    if (pt.x() < 1 || pt.y() < 1 || pt.x() > width() - 1 || pt.y() > height() - 1)
    {
        return;
    }
    emit cellClicked(QPoint(int((double)pt.x() / (double)width() * (double)m_fieldSize), int((double)pt.y() / (double)height() * (double)m_fieldSize)));
    mouseEvent->accept();
}

void GraphicsView::initGraphics(unsigned short fieldSize, unsigned short pointCapacity)
{
    Q_ASSERT(fieldSize > 1 && fieldSize <= 100);

    m_fieldSize = fieldSize;
    m_pointCapacity = pointCapacity;
    draw();
}

void GraphicsView::draw()
{
    if (!scene())
    {
        return;
    }
    scene()->clear();
    qDebug() << "clear scene";
    drawGrid();
}

void GraphicsView::drawGrid()
{
    double width = scene()->width();
    double height = scene()->height();

    for (unsigned short i = 0; i < m_fieldSize + 1; ++i)
    {
        scene()->addLine(0, i * (height / (double)m_fieldSize), width, i * (height / (double)m_fieldSize));
        scene()->addLine(i * (width / (double)m_fieldSize), 0, i * (width / (double)m_fieldSize), height);
    }
}

GraphicsItem *GraphicsView::addItem(const QUuid &uid, GraphicsItem *item)
{
    auto it = m_items.find(uid);
    if (it != m_items.end())
    {
        delete item;
        return getItem(uid);
    }
    m_items[uid] = item;
    scene()->addItem(item->getQGraphicsItem());
    return item;
}

GraphicsItem *GraphicsView::getItem(const QUuid &uid) const
{
    auto it = m_items.find(uid);
    return it == m_items.end() ? nullptr : *it;
}

unsigned short GraphicsView::getFieldSize() const
{
    return m_fieldSize;
}

unsigned short GraphicsView::getPointCapacity() const
{
    return m_pointCapacity;
}

}
