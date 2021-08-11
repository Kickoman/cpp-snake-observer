#include "observer.h"

#include <cstdio>

enum class MessageType : qint32
{
    FullState = 1,
    Update    = 2
};

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
        quint64 height = 0, width = 0;

        stream.startTransaction();

        qint32 type = 0;
        stream >> type;

        if (static_cast<MessageType>(type) == MessageType::FullState)
        {
            stream >> width >> height;
        }

        quint64 cellsCount = 0;
        stream >> cellsCount;

        std::vector<std::pair<std::pair<qint64, qint64>, std::pair<int, qint64>>> cellsInfo;
        for (quint64 i = 0; i < cellsCount; ++i)
        {
            qint64 x = 0, y = 0;
            int type = 0;
            qint64 id = 0;
            stream >> x >> y >> type >> id;
            cellsInfo.push_back(
                std::make_pair(
                    std::make_pair(x, y),
                    std::make_pair(type, id)
                    )
                );
        }

        if (!stream.commitTransaction())
        {
            printf("Bad trans\n");
            continue;
        }

        if (static_cast<MessageType>(type) == MessageType::FullState)
        {
            field.resize(height);
            for (auto &row : field)
                row.resize(width);
        }

        for (const auto & info : cellsInfo)
        {
            qint64 x = info.first.first;
            qint64 y = info.first.second;

            CellType type = static_cast<CellType>(info.second.first);
            qint64 id = info.second.second;

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
