#include <QDebug>

#include "proto/iprotomedia.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_protoMedia(nullptr),
    m_timer(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    m_timer->stop();
    delete m_timer;
    delete ui;
}

MainWindow::MainWindow(IProtoMedia *protoMedia): QMainWindow(nullptr),
    ui(new Ui::MainWindow),
    m_protoMedia(protoMedia),
    m_gameStatus(GameStatus::GsStopped)
{
    ui->setupUi(this);
    m_knownCommands.insert(CommandType::CtGameState);

    connect(ui->m_startStopBtn, SIGNAL(clicked()), this, SLOT(onStartStop()));
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    m_timer->start(100);
    m_protoMedia->postCommand(CommandType::CtGetGameState);
}

void MainWindow::onStartStop()
{
    if (m_gameStatus == GameStatus::GsStarted)
    {
        m_protoMedia->postCommand(CommandType::CtStopGame);
    }
    else
    {
        m_protoMedia->postCommand(CommandType::CtStartGame);
    }
    m_protoMedia->postCommand(CommandType::CtGetGameState);
}

bool MainWindow::handleCommand(CommandType ctype, const CommandData &data)
{
    switch (ctype)
    {
    case CommandType::CtGameState:
        onGameState(data);
        break;
    default:
        return false;
    }
    return true;
}

void MainWindow::tick()
{
    m_protoMedia->canProcess(this, m_knownCommands);
}

void MainWindow::onGameState(const CommandData &data)
{
    m_gameStatus = static_cast<GameStatus>(data[0].toInt());
    if (m_gameStatus == GameStatus::GsStarted)
    {
        ui->m_startStopBtn->setText("stop");
    }
    else
    {
        ui->m_startStopBtn->setText("start");
    }
}
