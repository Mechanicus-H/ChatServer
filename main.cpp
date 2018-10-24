#include "server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("octopus.png"));
    Server w;
    w.show();

    return a.exec();
}
