#include "observer.h"
#include <cstdio>
#include "gamemessage.h"

Observer::Observer(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &Observer::processInputUpdate);
    stream.setDevice(socket);
    stream.setVersion(QDataStream::Qt_5_0);
}

void Observer::connectTo(const QHostAddress &address, quint16 port)
{
    socket->connectToHost(address, port);
    printf("Connection...\n");
}

void Observer::processInputUpdate()
{
    while (socket->bytesAvailable())
    {
        bool ok = false;
        auto msg = GameMessage::deserialize(socket, &ok);
        if (!ok)
        {
            printf("Bad deserialization :(\n");
            continue;
        }

        if (msg.type == MessageType::FullState)
        {
            field.resize(int(msg.fieldHeight));
            for (auto & row : field)
                row.resize(int(msg.fieldWidth));
        }

        for (const auto & info : msg.cells)
        {
            qint64 x = info.coordX;
            qint64 y = info.coordY;

            CellType type = static_cast<CellType>(info.type);
            qint64 id = info.internalId;

            field[y][x].type = type;
            field[y][x].id = id;
        }

        printf("\033[%dA", field.size() + 1);
        for (int i = 0; i < field.size(); ++i)
        {
            for (int j = 0; j < field[i].size(); ++j)
                switch (field[i][j].type) {
                case CellType::Empty: printf("\u2591"); break;
                case CellType::Apple: printf("\033[1;33m\u25CF\033[0m"); break;
                case CellType::Snake: printf("\033[1;3%ldm\u2588\033[0m", field[i][j].id); break;
                case CellType::SnakeHead: printf("\u2593"); break;
                }
            putchar('\n');
        }
        static long long updateNumber = 0;
        ++updateNumber;
        printf("Updated: %ld\n", updateNumber);
    }
}
