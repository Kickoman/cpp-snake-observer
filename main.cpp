#include <QCoreApplication>
#include "observer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    auto observer = new Observer();
    observer->connectTo(QHostAddress::LocalHost, 9898);
    return a.exec();
}
