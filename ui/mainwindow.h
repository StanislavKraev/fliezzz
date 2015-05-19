#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSet>

#include "proto/iprotonode.h"
#include "engine/gamestatus.h"

namespace proto
{
    class IProtoMedia;
}

namespace Ui
{
    class MainWindow;
}

namespace ui
{

class MainWindow : public QMainWindow, public proto::IProtoNode
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    MainWindow(proto::IProtoMedia *protoMedia);
    virtual ~MainWindow();
public:
    virtual bool handleCommand(proto::CommandType ctype, const proto::CommandData &data);
private slots:
    void onStartStop();
    void tick();
    void onAddFly1();
private:
    void onGameState(const proto::CommandData &data);
    void onGameData(const proto::CommandData &data);
    void drawGrid(unsigned int fieldSize);
    void closeEvent();
private:
    Ui::MainWindow *ui;
    proto::IProtoMedia *m_protoMedia;
    QTimer *m_timer;
    QSet<proto::CommandType> m_knownCommands;

    engine::GameStatus m_gameStatus;
};

}

#endif // MAINWINDOW_H
