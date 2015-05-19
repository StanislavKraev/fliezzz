#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = 0);
    virtual ~GraphicsView();
public:
    void initGraphics(unsigned short fieldSize);
    void draw();
protected slots:
    virtual void mousePressEvent(QMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QMouseEvent * mouseEvent);
signals:
    void cellClicked(const QPoint &pt);
private:
    void drawGrid();
private:
    unsigned short m_fieldSize;
};

#endif // GRAPHICSVIEW_H
