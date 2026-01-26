#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void incomingConnection(qintptr socketDescriptor) override;
    void readClientData();
private:
    QSet<QTcpSocket*> userSockets;

};

#endif // SERVER_H
