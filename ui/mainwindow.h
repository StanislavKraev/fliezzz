#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSet>

#include "proto/iprotonode.h"
#include "engine/gamestatus.h"

class IProtoMedia;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public IProtoNode
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    MainWindow(IProtoMedia *protoMedia);
    virtual ~MainWindow();
public:
    virtual bool handleCommand(CommandType ctype, const CommandData &data);
private slots:
    void onStartStop();
    void tick();
private:
    void onGameState(const CommandData &data);
private:
    Ui::MainWindow *ui;
    IProtoMedia *m_protoMedia;
    QTimer *m_timer;
    QSet<CommandType> m_knownCommands;

    GameStatus m_gameStatus;
};

#endif // MAINWINDOW_H
