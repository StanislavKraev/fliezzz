#include <QApplication>

#include "proto/protomedia.h"
#include "engine/gamemanager.h"
#include "ui/mainwindow.h"

using namespace proto;
using namespace engine;
using namespace ui;

int main(int argc, char *argv[])
{
    ProtoMedia protoMedia;
    QApplication a(argc, argv);
    GameManager gameManager(&a, &protoMedia);
    gameManager.start();

    MainWindow w(&protoMedia);
    w.show();

    // todo: correct threads shutdown
    return a.exec();
}
