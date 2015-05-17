#include <QApplication>

#include "proto/protomedia.h"
#include "engine/gamemanager.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    ProtoMedia protoMedia;
    QApplication a(argc, argv);
    GameManager gameManager(&a, &protoMedia);
    gameManager.start();

    MainWindow w;
    w.show();

    protoMedia.postCommand(CommandType::CtStartGame);
    // todo: correct threads shutdown
    return a.exec();
}
