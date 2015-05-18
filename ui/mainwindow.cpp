#include <QDebug>
#include <QThread>
#include <QUuid>

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

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->m_graphicsView->setScene(scene);

    m_knownCommands.insert(CommandType::CtGameState);
    m_knownCommands.insert(CommandType::CtGameData);

    connect(ui->m_addFlyBtn, SIGNAL(clicked()), this, SLOT(onAddFly1()));
    connect(ui->m_startStopBtn, SIGNAL(clicked()), this, SLOT(onStartStop()));
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    qDebug() << QThread::currentThread()->currentThreadId();
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
    case CommandType::CtGameData:
        onGameData(data);
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

void MainWindow::onGameData(const CommandData &data)
{
    qDebug() << "data received";
    if (data[0] != 1)
    {
        qWarning() << "Incompatible packed";
        return;
    }
    ui->m_graphicsView->scene()->clear();
    ui->m_graphicsView->scene()->setSceneRect(0, 0, 100., 100.);
    const unsigned int fieldSize = data[1].toUInt();
    const unsigned int pointCapacity = data[2].toUInt();
    const unsigned int creaturesCount = data[3].toUInt();

    double width = ui->m_graphicsView->scene()->width();
    double height = ui->m_graphicsView->scene()->height();

    double flyWidth = width / (double) fieldSize * 0.6;
    double flyHeight = height / (double) fieldSize * 0.9;

    drawGrid(fieldSize);
    for (unsigned int creatureIndex = 0; creatureIndex < creaturesCount; ++creatureIndex)
    {
        QUuid uid = data[4 + creatureIndex * 4].toUuid();
        QString type = data[4 + creatureIndex * 4 + 1].toString();
        QPointF pos = data[4 + creatureIndex * 4 + 2].toPointF();
        int state = data[4 + creatureIndex * 4 + 3].toInt();

        ui->m_graphicsView->scene()->addEllipse(pos.x() * width - flyWidth / 2., pos.y() * height - flyHeight / 2., flyWidth, flyHeight);
    }
}

void MainWindow::drawGrid(unsigned int fieldSize)
{
    double width = ui->m_graphicsView->scene()->width();
    double height = ui->m_graphicsView->scene()->height();

    qDebug() << "scene: " << width << " x" << height;
    for (unsigned int i = 0; i < fieldSize + 1; ++i)
    {
        ui->m_graphicsView->scene()->addLine(0, i * (height / (double)fieldSize), width, i * (height / (double)fieldSize));
        ui->m_graphicsView->scene()->addLine(i * (width / (double)fieldSize), 0, i * (width / (double)fieldSize), height);
    }
}

void MainWindow::onAddFly1()
{
    CommandData data;
    data.append(QVariant("fly"));
    data.append(QVariant(QPoint(2, 2)));
    data.append(QVariant(60.));
    data.append(QVariant(0.1));
    data.append(QVariant(0.01));
    data.append(QVariant(3.));
    m_protoMedia->postCommand(CommandType::CtAddCreature, data);
}
