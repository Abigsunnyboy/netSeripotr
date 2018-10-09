#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <QtNetwork>

#include <tcpserverthread.h>
#include <mainwindow.h>
class MainWindow;
class tcpServerThread;

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);
    ~TcpServer();

   // TcpServer * tcpServer;
private slots:
   void deleteThread();

public:
      QList<int> socketList;
protected:


private:
     MainWindow * mainwindow;
     tcpServerThread* thread;
void incomingConnection (qintptr socketDescriptor);
};

#endif // TCPSERVER_H
