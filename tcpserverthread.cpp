#include "tcpserverthread.h"

tcpServerThread::tcpServerThread(qintptr socketDescriptor, QObject *parent) :
    QThread(parent)
{
  mainwindow = dynamic_cast<MainWindow *>(parent);
  this->socketDescriptor = socketDescriptor;
}

tcpServerThread::~tcpServerThread()
{
   socket->close();
}

void tcpServerThread::run(){

     socket = new MySocket(socketDescriptor, 0);

    if (!socket->setSocketDescriptor(socketDescriptor))
        return ;
    connect(socket, &MySocket::disconnected, this, &tcpServerThread::disconnectToHost);
    connect(socket, SIGNAL(revData(QString, QByteArray)), this, SLOT(recvData(QString, QByteArray)));
    connect(this, SIGNAL(sendDat(QByteArray, int)), socket, SLOT(sendMsg(QByteArray, int)));
    exec();

}

void tcpServerThread::sendData(QByteArray data, int id)
{
    if (data == "")
        return ;

    emit sendDat(data, id);
}

void tcpServerThread::recvData(QString peerAddr, QByteArray data)
{
    emit revData(peerAddr, data);
}

void tcpServerThread::disconnectToHost()
{
    emit disconnectTCP(socketDescriptor);
    socket->disconnectFromHost();
}
