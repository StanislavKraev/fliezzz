#include <QDebug>
#include <QThread>
#include <QTransform>
#include <QUuid>
#include <QGraphicsPixmapItem>

#include "proto/iprotomedia.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace proto;
using namespace engine;

namespace ui
{

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

MainWindow::MainWindow(proto::IProtoMedia *protoMedia): QMainWindow(nullptr),
    ui(new Ui::MainWindow),
    m_protoMedia(protoMedia),
    m_gameStatus(GameStatus::GsStopped)
{
    ui->setupUi(this);
    setFixedSize(650, 505);
    ui->m_graphicsView->setRenderHint(QPainter::Antialiasing);
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->m_graphicsView->setScene(scene);
    ui->m_graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->m_graphicsView->scene()->setSceneRect(0, 0, 500, 500);

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

bool MainWindow::handleCommand(proto::CommandType ctype, const proto::CommandData &data)
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

void MainWindow::onGameState(const proto::CommandData &data)
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

void MainWindow::onGameData(const proto::CommandData &data)
{
    if (data[0] != 2)
    {
        qWarning() << "Incompatible packed";
        return;
    }
    ui->m_graphicsView->scene()->clear();
    const unsigned int fieldSize = data[1].toUInt();
    const unsigned int pointCapacity = data[2].toUInt();
    const unsigned int creaturesCount = data[3].toUInt();


    double width = ui->m_graphicsView->scene()->width();
    double height = ui->m_graphicsView->scene()->height();

    double flyWidth = width / (double) fieldSize / (double)pointCapacity;
    double flyHeight = height / (double) fieldSize / (double)pointCapacity;

    drawGrid(fieldSize);
    for (unsigned int creatureIndex = 0; creatureIndex < creaturesCount; ++creatureIndex)
    {
//        QUuid uid = data[4 + creatureIndex * 5].toUuid();
//        QString type = data[4 + creatureIndex * 5 + 1].toString();
        QPointF pos = data[4 + creatureIndex * 5 + 2].toPointF();
//        int state = data[4 + creatureIndex * 5 + 3].toInt();
        double angle = data[4 + creatureIndex * 5 + 4].toDouble();

        QTransform transform;
        QTransform trans = transform.rotate(90 + angle * 180. / 3.14159265);
        QGraphicsPixmapItem *item = ui->m_graphicsView->scene()->addPixmap(QPixmap(QString(":/ui/fly.png")).transformed(trans));

        item->setTransformationMode(Qt::SmoothTransformation);
        item->setPos(pos.x() * width / (double)fieldSize - flyWidth / 2., pos.y() * height / (double)fieldSize - flyHeight / 2.);
        item->setScale(1.0 / 30. * flyHeight);
    }
}

void MainWindow::drawGrid(unsigned int fieldSize)
{
    double width = ui->m_graphicsView->scene()->width();
    double height = ui->m_graphicsView->scene()->height();

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
    data.append(QVariant(10. + (double)rand() / RAND_MAX * 40.));
    data.append(QVariant(0.5 + (double)rand() / RAND_MAX * 1.5));
    data.append(QVariant(0.01));
    data.append(QVariant(0.5 + (double)rand() / RAND_MAX * 3.));
    m_protoMedia->postCommand(CommandType::CtAddCreature, data);
}

}
