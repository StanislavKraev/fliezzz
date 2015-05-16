#include <QApplication>

#include "proto/protomedia.h"
#include "engine/gamemanager.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    ProtoMedia protoMedia;
    GameManager gameManager(&protoMedia);

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    protoMedia.handleCommand(CommandType::CtStartGame);
    return a.exec();
}
