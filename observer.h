#ifndef OBSERVER_H
#define OBSERVER_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QDataStream>

enum class CellType : int
{
    Empty       = 0,
    Snake       = 1,
    SnakeHead   = 2,
    Apple       = 3
};

struct Cell
{
    CellType type;
    qint64 id = 0;
};

class Observer : public QObject
{
    Q_OBJECT
public:
    explicit Observer(QObject *parent = nullptr);

    void connectTo(const QHostAddress & address, quint16 port);

private slots:
    void processInputUpdate();

private:
    QTcpSocket *socket = nullptr;
    QDataStream stream;

    QVector<QVector<Cell>> field;
};

#endif // OBSERVER_H
