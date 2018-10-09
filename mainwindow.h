#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QtNetwork>
#include "tcpserverthread.h"
#include "tcpserver.h"
#include <QListWidget>
#include <QFileDialog>


namespace Ui {
class MainWindow;
}

class tcpServerThread;
class TcpServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initUI();
    void udpServerset();
    void tcpClientset();
    void tcpServerset();

    static QStringList getLocalIP();
    static bool isIP(const QString &ip);
    QString charToHex(const QByteArray str);
    QTimer *timer;

  //服务器端设置

    TcpServer *tcpServer;

    QLabel *lableTCPstatus = new QLabel(this);

    QLabel *lableTCPIP = new QLabel(this);

    QLabel *lableTCPPort = new QLabel(this);

    QLabel *lableSocketStatus = new  QLabel(this);

  //客户端设置
    QTcpSocket *tcpClient;
    QLabel* lableSocketStatus_client = new QLabel(this);

    QLabel *lableClient_remoteIP= new QLabel(this);
    QLabel *lableClient_remotePort= new QLabel(this);
   // QLabel *lableClient= new QLabel(this);

    //udp 端设置
    QUdpSocket *udpSocket;
    QLabel *lableUDPsocketStatus;
    QLabel *lableUDPsocketIP;
private:

signals:

    void sendData(QByteArray data, int id);

private slots:

    //tcp槽函数
    void on_pushButton_TCPServerOpen_clicked();

    void displayError(QAbstractSocket::SocketError);

    void showConnection();
    void showDisconnection(int socketDescriptor);

    void onSocketStateChange(QAbstractSocket::SocketState  socketstate);
//    void onSocketReadyread();


    //客户端槽函数设置
    void onConnectedtoServer();
    void onDisconnectedtoServer();
    void ClientReadyread();
    void on_pushButton_TCPClientLink_clicked();


   //udp 槽函数设置
    void onUDPsocketReadyRead();

    void on_pushButton_UDPopen_clicked();

//通用槽函数
    void selectModule(int index);

    void on_pushButton_Senddata_clicked();

    void on_pushButton_clearReceive_clicked();

    void on_pushButton_Cllearinput_clicked();

    void on_checkBox_SendHex_clicked(bool checked);

    void on_pushButton_SaveData_clicked();

    void on_checkBox_TimerSend_clicked(bool checked);

    void on_textEdit_ReceiveData_textChanged();


    QString getCurrentTime();

    void on_checkBox_SendFileData_clicked(bool checked);

    void on_pushButton_Sendfile_clicked();

public slots:
    void revData(QString peerHost, QByteArray data);

private:
    Ui::MainWindow *ui;

    tcpServerThread *serverThread;

    int count;
    QString dataTime;
protected:
    void closeEvent(QCloseEvent *event);
signals:
       void mainwindowClose();
       void TCPSendDataClicked();
//    void sendData(const QString &ip, int port, const QString &data);
//    void receiveData(const QString &ip, int port, const QString &data);
       void sendfile_line();
};



#endif // MAINWINDOW_H


