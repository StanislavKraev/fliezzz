#include <QDebug>
#include <QThread>
#include <QTransform>
#include <QUuid>
#include <QGraphicsPixmapItem>
#include <QDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QTime>

#include "proto/iprotomedia.h"

#include "flygraphicsitem.h"
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
    move(50, 50);
    ui->m_graphicsView->setRenderHint(QPainter::Antialiasing);

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->m_graphicsView->setScene(scene);

    ui->m_graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->m_graphicsView->scene()->setSceneRect(0, 0, 500, 500);

    connect(ui->m_graphicsView, SIGNAL(cellClicked(QPoint)), this, SLOT(onCellClick(QPoint)));

    m_knownCommands.insert(CommandType::CtGameState);
    m_knownCommands.insert(CommandType::CtGameData);
    m_knownCommands.insert(CommandType::CtGameConfig);
    m_knownCommands.insert(CommandType::CtGameStats);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->fastBtn);
    group->addButton(ui->normalBtn);
    group->addButton(ui->stupidBtn);
    ui->fastBtn->setChecked(true);

    connect(ui->m_startStopBtn, SIGNAL(clicked()), this, SLOT(onStartStop()));
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    connect(ui->fieldSizeSlider, SIGNAL(sliderReleased()), this, SLOT(onFieldSizeChange()));
    connect(ui->spotCapacitySlider, SIGNAL(sliderReleased()), this, SLOT(onSpotCapacityChange()));

    m_timer->start(100);
    m_protoMedia->postCommand(CommandType::CtGetGameState);
    m_protoMedia->postCommand(CommandType::CtGetConfig);
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
    if (m_gameStatus == GameStatus::GsStarted)
    {
        getStats();
    }
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
    case CommandType::CtGameConfig:
        onConfig(data);
        break;
    case CommandType::CtGameStats:
        onStats(data);
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
    if (data[0] != 4)
    {
        qWarning() << "Incompatible packet";
        return;
    }

    const unsigned int creaturesCount = data[1].toUInt();

    double width = ui->m_graphicsView->scene()->width();
    double height = ui->m_graphicsView->scene()->height();

    double fieldSize = (double)ui->m_graphicsView->getFieldSize();
    double pointCapacity = (double)ui->m_graphicsView->getPointCapacity();

    double flyWidth = width / fieldSize / sqrt(pointCapacity);
    double flyHeight = height / fieldSize / sqrt(pointCapacity);


    for (unsigned int creatureIndex = 0; creatureIndex < creaturesCount; ++creatureIndex)
    {
        QUuid uid = data[2 + creatureIndex * 6].toUuid();
//        QString type = data[2 + creatureIndex * 6 + 1].toString();
        QPointF pos = data[2 + creatureIndex * 6 + 2].toPointF();
        int state = data[2 + creatureIndex * 6 + 3].toInt();
        double angle = data[2 + creatureIndex * 6 + 4].toDouble();
        double thinkTime = data[2 + creatureIndex * 6 + 5].toDouble();

        GraphicsItem *item = ui->m_graphicsView->getItem(uid);
        if (!item)
        {
            item = ui->m_graphicsView->addItem(uid, new FlyGraphicsItem(uid, flyWidth, flyHeight, thinkTime));
        }
        if (item)
        {
            QPoint movePt = QPoint(pos.x() * width / (double)ui->m_graphicsView->getFieldSize(),
                                   pos.y() * height / (double)ui->m_graphicsView->getFieldSize());
            item->update(movePt, 90 + angle * 180. / 3.14159265, state);

        }
    }
}

void MainWindow::addFly(const QPoint &pt)
{
    qDebug() << QTime::currentTime().toString() << " add fly";
    double thinkMultiplier = ui->fastBtn->isChecked() ? 0.5 : (ui->normalBtn->isChecked() ? 2. : 5.);
    double thinkMin = ui->fastBtn->isChecked() ? 0.2 : (ui->normalBtn->isChecked() ? 1. : 3.);
    CommandData data;
    data.append(QVariant("fly"));
    data.append(QVariant(pt));
    data.append(QVariant(10. + (double)rand() / RAND_MAX * 40.));
    data.append(QVariant(0.5 + (double)rand() / RAND_MAX * 1.5));
    data.append(QVariant(0.01));
    data.append(QVariant(thinkMin + (double)rand() / RAND_MAX * thinkMultiplier));
    m_protoMedia->postCommand(CommandType::CtAddCreature, data);
}

void MainWindow::onAddFly1()
{
    addFly(QPoint(4, 4));
}

void MainWindow::onCellClick(const QPoint &pt)
{
    addFly(pt);
}

void MainWindow::onConfig(const proto::CommandData &data)
{
    // todo: skip if config actually was not changed

    qDebug() << "on config - reset scene";
    const unsigned int fieldSize = data[0].toUInt();
    const unsigned int pointCapacity = data[1].toUInt();
    ui->m_graphicsView->initGraphics(fieldSize, pointCapacity);
}

void MainWindow::onFieldSizeChange()
{
    CommandData data;
    data.append(QVariant(ui->fieldSizeSlider->value()));
    data.append(QVariant(ui->spotCapacitySlider->value()));
    m_protoMedia->postCommand(CommandType::CtChangeConfig, data);
}

void MainWindow::onSpotCapacityChange()
{
    CommandData data;
    data.append(QVariant(ui->fieldSizeSlider->value()));
    data.append(QVariant(ui->spotCapacitySlider->value()));
    m_protoMedia->postCommand(CommandType::CtChangeConfig, data);
}

void MainWindow::getStats()
{
    m_protoMedia->postCommand(CommandType::CtGetStats);
}

void MainWindow::onStats(const proto::CommandData &data)
{
    int count = data.at(0).toInt();
    if (!count)
    {
        return;
    }
    qDebug() << "show stats";
    QDialog *newDialog = new QDialog(this);
    newDialog->setLayout(new QVBoxLayout());
    QPlainTextEdit *edit = new QPlainTextEdit(newDialog);
    newDialog->layout()->addWidget(edit);
    QString text;
    for (int i = 0; i < count; ++i)
    {
        QUuid uid = data.at(1 + i * 4).toUuid();
        double averageVel = data.at(1 + i * 4 + 1).toDouble();
        double mileage = data.at(1 + i * 4 + 2).toDouble();
        bool isDead = data.at(1 + i * 4 + 3).toBool();

        qDebug() << uid << ": " << averageVel << " & " << mileage << " & " << isDead;
        text += QString::number(i + 1) + ": v=" + QString::number(averageVel) + "; s=" + QString::number(mileage) + (isDead ? "; dead" : "; alive") + "<br/>";
    }
    edit->document()->setHtml(text);
    newDialog->show();
    newDialog->resize(400, 600);
    newDialog->setFixedSize(400, 600);
    newDialog->exec();
}

}
