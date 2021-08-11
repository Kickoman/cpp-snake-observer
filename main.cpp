#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include "observer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Snake client");
    QCoreApplication::setApplicationVersion("0.2");

    QCommandLineParser parser;
    parser.setApplicationDescription("Basic snake client (observe only)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption addressOpt(QStringList() << "a" << "address",
        "Server address", "address", "127.0.0.1");
    parser.addOption(addressOpt);

    QCommandLineOption portOpt(QStringList() << "p" << "port",
        "Server port", "port", "9898");

    parser.process(a);

    QHostAddress address = QHostAddress::LocalHost;
    quint16 port = 9898;

    if (parser.isSet(addressOpt))
    {
        address = QHostAddress(parser.value(addressOpt));
        if (address.isNull())
        {
            qDebug() << "Can't parse address" << parser.value(addressOpt);
            return 1;
        }
    }

    if (parser.isSet(portOpt))
    {
        bool ok = false;
        port = parser.value(portOpt).toUShort(&ok);
        if (!ok)
        {
            qDebug() << "Can't parse port" << parser.value(portOpt);
            return 2;
        }
    }

    auto observer = new Observer();
    observer->connectTo(address, port);
    return a.exec();
}
