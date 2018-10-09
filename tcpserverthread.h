#ifndef TCPSERVERTHREAD_H
#define TCPSERVERTHREAD_H

#include <mainwindow.h>
#include <QtNetwork>
#include <mysocket.h>

class MainWindow;

//#endif // TCPSERVERTHREAD_H

class tcpServerThread : public QThread
{
    Q_OBJECT
public:
    tcpServerThread(qintptr socketDescriptor, QObject *parent = 0);
    ~tcpServerThread();

    void run();

public:


signals:
    void revData(QString, QByteArray);
    void sendDat(QByteArray data, int id);
    void disconnectTCP(int );

public slots:
    void sendData(QByteArray data, int id);
    void recvData(QString, QByteArray);
    void disconnectToHost();

  //  void deleteLater();

private:
    MainWindow * mainwindow;
    MySocket *socket;


    int socketDescriptor;
};
#endif
// SOCKETTHREAD

