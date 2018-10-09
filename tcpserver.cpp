#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent)
{
    mainwindow = dynamic_cast<MainWindow *>(parent);

}
TcpServer::~TcpServer()
{

}
void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "Connecting..." << socketDescriptor;
    thread = new tcpServerThread(socketDescriptor, 0);

    socketList.append(socketDescriptor);

    connect(thread, SIGNAL(started()), mainwindow, SLOT(showConnection()));
    connect(thread, SIGNAL(disconnectTCP(int)), mainwindow, SLOT(showDisconnection(int)));
    connect(thread, SIGNAL(revData(QString, QByteArray)), mainwindow, SLOT(revData(QString, QByteArray)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(mainwindow, SIGNAL(sendData(QByteArray, int)), thread, SLOT(sendData(QByteArray, int)));

    connect(mainwindow, SIGNAL(mainwindowClose()), this, SLOT(deleteThread()));


    thread->start ();
}

void TcpServer::deleteThread(){
    if(thread->isRunning())
    {
        thread->quit();
    }
}
