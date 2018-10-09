#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), count(0), timer(new QTimer)
{
    ui->setupUi(this);

    tcpClient = new QTcpSocket(this);


    QString name = QString::fromLocal8Bit("网络通讯助手");
    QString data = "10.9";
    QString version = "v2.1";
    QString aurthor = "zhaoliang";
    setWindowTitle(name+" "+data +" "+ version+" "+aurthor);
    setFixedHeight(660);
    setFixedWidth(900);
    initUI();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: initUI(){

    //通用设置
    QStringList ips = getLocalIP();
    ui->comboBox_UDPAdress->addItems(ips);
    ui->lineEdit_UDPPort->setText("6000");//udp默认设置

    ui->comboBox_TCPClientLocalAdress->addItems(ips);
    ui->comboBox_TCPClientRemoteAdress->addItem("127.0.0.1");//tcpclient 默认设置
    ui->comboBox_TCPClientRemoteAdress->addItem("192.168.184.1");//tcpclient 默认设置

    ui->lineEdit_clientRemotePort->setText("6000");

    ui->comboBox_TCPServerLocalAdress->addItems(ips);
    ui->lineEdit_TCPServerRemotePort->setText("6000");
    ui->tabWidget->setCurrentIndex(0);

    ui->lineEdit_UDPtargetAdress->setText("127.0.0.1");
    ui->lineEdit_UDPtargetPort->setText("6000");


    selectModule(ui->tabWidget->currentIndex());
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(selectModule(int)));

    ui->lineEdit_SendInterval->setText("1000");

    ui->radioButton_singalSend->setChecked(true);


    QRegExp num("[0-9]+$");//端口号只能为数字
    ui->lineEdit_UDPPort->setValidator(new QRegExpValidator(num));
    ui->lineEdit_TCPServerRemotePort->setValidator(new QRegExpValidator(num));
    ui->lineEdit_clientRemotePort->setValidator(new QRegExpValidator(num));
    ui->lineEdit_SendInterval->setValidator(new QRegExpValidator(num));

    QRegExp HostAddress ("[0-9.]+$");
    ui->comboBox_TCPClientLocalAdress->setValidator(new QRegExpValidator(HostAddress));
    ui->comboBox_TCPClientRemoteAdress->setValidator(new QRegExpValidator(HostAddress));
    ui->comboBox_TCPServerLocalAdress->setValidator(new QRegExpValidator(HostAddress));
    ui->comboBox_TCPClientLocalAdress->setValidator(new QRegExpValidator(HostAddress));
    ui->comboBox_UDPAdress->setValidator(new QRegExpValidator(HostAddress));
    ui->lineEdit_UDPtargetAdress->setValidator(new QRegExpValidator(HostAddress));


    ui->checkBox_SendAnswer->setCheckable(false);
    ui->checkBox_SendAddtional->setCheckable(false);
    connect(this,SIGNAL(sendfile_line()),this,SLOT(on_pushButton_Senddata_clicked()));
}

void MainWindow::selectModule(int index){

    switch (index) {
    case 0:
        udpServerset();
        break;
    case 1:
        tcpClientset();
        break;
    case 2:
        tcpServerset();
        break;
    default:
        break;
    }
}

void MainWindow::udpServerset(){
    // ui->statusBar->clearMessage();
    udpSocket= new QUdpSocket(this);
    connect (udpSocket,SIGNAL (readyRead()),this,SLOT(onUDPsocketReadyRead()));

    lableTCPstatus->setVisible(false);
    lableTCPIP->setVisible(false);
    lableTCPPort->setVisible(false);
    lableSocketStatus->setVisible(false);

    lableClient_remoteIP->setVisible(false);
    lableClient_remotePort->setVisible(false);
    lableSocketStatus_client->setVisible(false);
}

void MainWindow::tcpClientset(){
    //客户端设置
    lableClient_remoteIP->setVisible(true);
    lableClient_remotePort->setVisible(true);
    lableSocketStatus_client->setVisible(true);

    ui->statusBar->addWidget(lableSocketStatus_client);
    ui->statusBar->addWidget(lableClient_remoteIP);
    ui->statusBar->addWidget(lableClient_remotePort);

    lableTCPstatus->setVisible(false);
    lableTCPIP->setVisible(false);
    lableTCPPort->setVisible(false);
    lableSocketStatus->setVisible(false);

    connect(tcpClient,SIGNAL(connected()),this,SLOT(onConnectedtoServer()));
    connect(tcpClient,SIGNAL(disconnected()),this,SLOT(onDisconnectedtoServer()));
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(ClientReadyread()));


}

void MainWindow::tcpServerset(){

    lableTCPstatus->setVisible(true);
    lableTCPIP->setVisible(true);
    lableTCPPort->setVisible(true);
    lableSocketStatus->setVisible(true);

    ui->statusBar->addWidget(lableTCPstatus);
    ui->statusBar->addWidget(lableTCPIP);
    ui->statusBar->addWidget(lableTCPPort);
    ui->statusBar->addWidget(lableSocketStatus);

    lableClient_remoteIP->setVisible(false);
    lableClient_remotePort->setVisible(false);
    lableSocketStatus_client->setVisible(false);

    tcpServer = new TcpServer(this);


}

void MainWindow::on_pushButton_clearReceive_clicked()
{
    ui->textEdit_ReceiveData->clear();
}


void MainWindow::on_pushButton_Senddata_clicked()
{    /* if send message is null return */
    if (ui->lineEdit_Sendedit->text() == "") {
        QMessageBox::information(NULL,
                                 QString::fromLocal8Bit("注意"),
                                 QString::fromLocal8Bit("发送内容为空"),
                                 QMessageBox::Yes);

        return ;
    }

    //读取发送文本
    int module =  ui->tabWidget->currentIndex();
    QString message = ui->lineEdit_Sendedit->text();
    dataTime = getCurrentTime();
    QByteArray str;

    if (ui->checkBox_SendHex->checkState())
    {
        if (message.size()%2!=0)
        {
            message = QString::number(0)+message;
        }
        str = QByteArray::fromHex (message.toLatin1().data());

    }
    else
    {
        str = message.toUtf8();
    }
    if (ui->checkBox_TimerSend->isTristate())
    {
        qDebug()<<"timer";
    }
    //UDP 地址
    QString targetIP = ui->lineEdit_UDPtargetAdress->text();
    QHostAddress targetADDr (targetIP);
    quint16 targetPort = ui->lineEdit_UDPtargetPort->text().toInt();

    switch (module) {
    case 0:
        // qDebug()<<"udpServer send data";

        if (ui->radioButton_singalSend->isChecked())
        {
            udpSocket->writeDatagram(str,targetADDr,targetPort);

            ui->textEdit_ReceiveData->append(dataTime +QString::fromLocal8Bit("udp单播发送：" )+message);
        }
        else if (ui->radioButton_multiSend->isChecked()){//广播
            udpSocket->writeDatagram(str,QHostAddress::Broadcast,targetPort);//广播地址一般255.255.255.255，即QHostAddress::Broadcast

            ui->textEdit_ReceiveData->append(dataTime +QString::fromLocal8Bit("udp广播发送：" )+message);
        }

        break;
    case 1:
        //  tcpClientset();
        tcpClient->write(str);
        ui->textEdit_ReceiveData->setTextColor(QColor ("black"));

        ui->textEdit_ReceiveData->append(dataTime +QString::fromLocal8Bit("客户端发送: ") +message);

        break;
    case 2:

        if (ui->checkBox_toAllClient->checkState())
        {
            for (int i = 0; i < tcpServer->socketList.size(); i++)
                emit sendData(str,ui->listWidget_clientInfo->item(i)->text().toInt());
        }
        else
        {
            if (ui->listWidget_clientInfo->count() != 0){
                ui->listWidget_clientInfo->setCurrentRow(0);
                emit sendData(str,ui->listWidget_clientInfo->currentItem()->text().toInt());
            }
            else{QMessageBox::information(NULL,
                                          QString::fromLocal8Bit("注意"),
                                          QString::fromLocal8Bit("没有客户端连接"),
                                          QMessageBox::Yes);

                return ;

            }
        }

        ui->textEdit_ReceiveData->setTextColor(QColor ("red"));

        ui->textEdit_ReceiveData->append(dataTime +QString::fromLocal8Bit("服务端发送: ") +message);
        break;
    default:
        break;
    }
}



QStringList MainWindow:: getLocalIP(){

    //    QString hostName = QHostInfo::localHostName();
    //    QHostInfo hostName = QHostInfo::fromName(hostName);

    QStringList ips;
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, addrs) {
        QString ip = addr.toString();
        if (MainWindow::isIP(ip)) {
            ips << ip;
        }
    }
    //优先取192开头的IP,如果获取不到IP则取127.0.0.1
    QString ip = "127.0.0.1";
    foreach (QString str, ips) {
        if (str.startsWith("192.168.1") || str.startsWith("192")) {
            ip = str;
            ips.removeOne(ip);
            break;
        }
    }
    ips.insert(0,ip);//将192开头的设为默认IP

    qDebug()<< ips;
    return ips;
}


bool MainWindow::isIP(const QString &ip)//判断是否是IP地址
{
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}


//tcp 服务端程序



void MainWindow :: on_pushButton_TCPServerOpen_clicked(){//打开服务器
    QString IP = ui->comboBox_TCPServerLocalAdress->currentText();
    quint16 port = ui->lineEdit_TCPServerRemotePort->text().toInt();//取得服务器地址和端口号

    if (isIP(IP)){
        if (ui->pushButton_TCPServerOpen->text()==QString::fromLocal8Bit("监听"))
        {

            QHostAddress addr(IP);

            tcpServer->listen(addr,port);//监听

            ui->pushButton_TCPServerOpen->setText(QString::fromLocal8Bit("关闭服务器"));
            lableTCPstatus->setText(QString::fromLocal8Bit("服务器状态：正在监听..."));

            lableTCPIP->setText(QString::fromLocal8Bit("服务器地址：")+tcpServer->serverAddress().toString());
            lableTCPPort->setText(QString::fromLocal8Bit("服务器端口：")+QString::number(tcpServer->serverPort()));

        }
        else{
            ui->pushButton_TCPServerOpen->setText(QString::fromLocal8Bit("监听"));

            tcpServer->close();

            lableTCPstatus->setText(QString::fromLocal8Bit("服务器状态：服务器关闭"));
            lableTCPIP->setText(QString::fromLocal8Bit("服务器地址：")+tcpServer->serverAddress().toString());
            lableTCPPort->setText(QString::fromLocal8Bit("服务器端口：")+QString::number(tcpServer->serverPort()));
        }
    }
    else{
        QMessageBox::information(NULL,
                                 QString::fromLocal8Bit("注意"),
                                 QString::fromLocal8Bit("Ip不正确"),
                                 QMessageBox::Yes);
    }

}


void MainWindow::showConnection()
{
    count++;

    /* change connect number while connection is connecting */
    ui->labCount->setText(QString::fromLocal8Bit("共")+" "+QString::number(count)+" "+QString::fromLocal8Bit("个连接"));
    /* add socket object that join in */
    ui->listWidget_clientInfo->addItem(QString::number(tcpServer->socketList.last()));


}

void MainWindow::showDisconnection(int socketDescriptor)
{
    count--;

    /* remove disconnect socketdescriptor from list */
    tcpServer->socketList.removeAll(socketDescriptor);

    /* reload combobox */

    ui->listWidget_clientInfo->clear();
    for (int i = 0; i < tcpServer->socketList.size(); i++) {
        ui->listWidget_clientInfo->addItem(QString("%1").arg(tcpServer->socketList.at(i)));
    }

    //change connect number while connection is disconnecting
    ui->labCount->setText(QString::fromLocal8Bit("共")+" "+QString::number(count)+" "+QString::fromLocal8Bit("个连接"));
}


void MainWindow::revData(QString peerAddr, QByteArray data)
{
    QString msg;
    if (!ui->checkBox_TerminalReceive->checkState())
    {
        dataTime = getCurrentTime();
        if (ui->checkBox_ReceiveHex->isChecked()) {
            msg= charToHex(data);

        }else{
            msg = QString::fromLocal8Bit(data);
        }

        peerAddr.insert(peerAddr.size(), ": ");
        msg.prepend(peerAddr);
        ui->textEdit_ReceiveData->setTextColor(QColor ("blue"));
        ui->textEdit_ReceiveData->append(dataTime +" "+ msg);
    }

}



void MainWindow::onSocketStateChange(QAbstractSocket::SocketState  socketstate){//读取socket状态的改变

    switch (socketstate) {
    case  QAbstractSocket::HostLookupState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"HostLookupState");
        break;
    case  QAbstractSocket::UnconnectedState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"UnconnectedState");
        break;
    case  QAbstractSocket::ConnectingState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ConnectingState");
        break;
    case  QAbstractSocket::ConnectedState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ConnectedState");
        break;
    case  QAbstractSocket::BoundState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"BoundState");
        break;
    case  QAbstractSocket::ClosingState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ClosingState");
        break;
    case  QAbstractSocket::ListeningState :
        lableSocketStatus->setText("socket" +QString::fromLocal8Bit("状态:")+"ListeningState");
        break;
    default:
        break;

    }
}


void MainWindow::displayError(QAbstractSocket::SocketError){

    QMessageBox::warning(this,"Warning","socket error",QMessageBox::Yes);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<event;

    emit mainwindowClose();
}

//客户端代码
void MainWindow::onConnectedtoServer(){

    // qDebug()<<"";

    lableClient_remoteIP->setText(QString::fromLocal8Bit("远程服务器地址和端口：")+tcpClient->peerAddress().toString()
                                  +"::"+QString::number(tcpClient->peerPort()));
    lableSocketStatus_client->setText(QString::fromLocal8Bit("连接状态：已连接"));

}

void MainWindow::onDisconnectedtoServer(){
    // lableSocketStatus_client->setText(QString::fromLocal8Bit("连接状态：未连接"));
    lableClient_remoteIP->setText(QString::fromLocal8Bit("远程服务器地址和端口："));
    lableSocketStatus_client->setText(QString::fromLocal8Bit("连接状态：未连接"));
}
void MainWindow::ClientReadyread(){
    if (!ui->checkBox_TerminalReceive->checkState()){
        dataTime = getCurrentTime();
        QByteArray buff = tcpClient->readAll();
        qDebug()<< buff;
        QString message;
        ui->textEdit_ReceiveData->setTextColor(QColor ("green"));

        if (ui->checkBox_ReceiveHex->checkState())
        {
            message = charToHex(buff);
            message = (QString::fromLocal8Bit("接收自服务器：")
                       +tcpClient->peerAddress().toString()+"::"+QString::number(tcpClient->peerPort())+":  "+message );
        }
        else{
            message =(QString::fromLocal8Bit("接收自服务器：")
                      +tcpClient->peerAddress().toString()+"::"+QString::number(tcpClient->peerPort())+":  "+buff ).toUtf8();
            //  ui->textEdit_ReceiveData->append("<font color=\"#0000FF\"> \message\ </font>");
        }

        ui->textEdit_ReceiveData->append(dataTime +" "+message);
    }
}


void MainWindow::on_pushButton_TCPClientLink_clicked()//连接或者断开客户端连接
{
    QString addr = ui->comboBox_TCPClientRemoteAdress->currentText();
    quint16 port = ui->lineEdit_TCPServerRemotePort->text().toUInt();
    if (isIP(addr)){
        if(tcpClient->state()==QAbstractSocket::UnconnectedState &&ui->pushButton_TCPClientLink->text() == QString::fromLocal8Bit("连接") ){
            tcpClient->connectToHost(addr,port);
            ui->pushButton_TCPClientLink->setText(QString::fromLocal8Bit("断开"));
            // qDebug()<<tcpClient->state();
        }

        else
        {
            ui->pushButton_TCPClientLink->setText(QString::fromLocal8Bit("连接"));
            tcpClient->disconnectFromHost();
            //  qDebug()<<tcpClient->state();
        }
    }
    else{
        QMessageBox::information(NULL,
                                 QString::fromLocal8Bit("注意"),
                                 QString::fromLocal8Bit("Ip不正确"),
                                 QMessageBox::Yes);
    }
}

//UDP 代码

void MainWindow::on_pushButton_UDPopen_clicked()
{
    quint16 port = ui->lineEdit_UDPPort->text().toInt();
    QString addr = ui->comboBox_UDPAdress->currentText();

    if (isIP(addr)){
        if (ui->pushButton_UDPopen->text() == QString::fromLocal8Bit("绑定")&&udpSocket->bind(port))//绑定成功
        {
            // udpSocket->bind(QHostAddress::LocalHost, port);
            //   qDebug()<< udpSocket->state();

            ui->pushButton_UDPopen->setText(QString::fromLocal8Bit("解除绑定"));

            // lableUDPsocketStatus->setText(QString::fromLocal8Bit("绑定成功"));
        }
        else {
            ui->pushButton_UDPopen->setText(QString::fromLocal8Bit("绑定"));
            udpSocket->abort();//断开连接
            //  qDebug()<< udpSocket->state();
            // lableUDPsocketStatus->setText(QString::fromLocal8Bit("未绑定"));
        }
    }
    else{
        QMessageBox::information(NULL,
                                 QString::fromLocal8Bit("注意"),
                                 QString::fromLocal8Bit("Ip不正确"),
                                 QMessageBox::Yes);
    }
}

void MainWindow::onUDPsocketReadyRead(){
    if (!ui->checkBox_TerminalReceive->checkState()){
        dataTime = getCurrentTime();

        while (udpSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(udpSocket->pendingDatagramSize());

            QHostAddress peerADDr;
            quint16 peerPort;
            udpSocket->readDatagram(datagram.data(),datagram.size(),&peerADDr,&peerPort);//读取数据并获得数据来源的地址和端口
            QString tempPeer = peerADDr.toString();
            tempPeer =tempPeer.remove(0,7);

            QString str = datagram.data();
            qDebug()<<str;
            if (ui->checkBox_ReceiveHex->checkState())
            {
                str = charToHex(datagram);

            }
            else{
                str = str.toUtf8();
            }
            QString peer = (QString::fromLocal8Bit("由")+ tempPeer+"::"+QString::number(peerPort));

            ui->textEdit_ReceiveData->append(dataTime +" "+peer+" "+str);
        }
    }
}

void MainWindow::on_pushButton_Cllearinput_clicked()
{
    ui->lineEdit_Sendedit->clear();
}

QString MainWindow::charToHex(const QByteArray buff){
    QString strDis;
    QString str = buff.toHex();//以十六进制显示
    str = str.toUpper ();//转换为大写
    for(int i = 0;i<str.length ();i+=2)//填加空格
    {
        QString st = str.mid (i,2);
        strDis += st;
        strDis += " ";
    }
    return strDis;
}

void MainWindow::on_checkBox_SendHex_clicked(bool checked)
{
    if(checked)
    {
        QRegExp abcd("[a-fA-F0-9]+$");
        ui->lineEdit_Sendedit->setPlaceholderText("input a-zA-Z0-9");
        ui->lineEdit_Sendedit->setValidator(new QRegExpValidator(abcd));
    }
    else{
        ui->lineEdit_Sendedit->setPlaceholderText("input what you want");
    }
}

void MainWindow::on_pushButton_SaveData_clicked()
{
    QString filePath = "./data.txt";

    QFile file(filePath);
    //   if (file.exists()) {
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString data = ui->textEdit_ReceiveData->document()->toPlainText();
        stream << data;

    }
    else {
        QString title = title.fromLocal8Bit("文件打开错误");
        QString information = information.fromLocal8Bit("请在当前目录下创建data.txt");
        QMessageBox::warning(this, title, information,QMessageBox::Ok);
    }
}

void MainWindow::on_checkBox_TimerSend_clicked(bool checked)
{
    if (!ui->lineEdit_SendInterval->text().isEmpty()&&!ui->lineEdit_Sendedit->text().isEmpty()){
        if (checked){
            ui-> pushButton_TCPServerOpen->setEnabled(false);
            ui->pushButton_UDPopen->setEnabled(false);
            ui->pushButton_TCPClientLink->setEnabled(false);
            ui->lineEdit_SendInterval->setEnabled(false);
            timer = new QTimer();
            connect(timer, SIGNAL(timeout()), this, SLOT( on_pushButton_Senddata_clicked()));
            int time = ui->lineEdit_SendInterval->text().toInt();
            timer->start(time);
        }

        else{
            timer->stop();
            ui-> pushButton_TCPServerOpen->setEnabled(true);
            ui->pushButton_UDPopen->setEnabled(true);
            ui->pushButton_TCPClientLink->setEnabled(true);
            ui->lineEdit_SendInterval->setEnabled(true);
        }
    }
    else{
        ui->checkBox_TimerSend->setChecked(false);

        QMessageBox::information(this,QString::fromLocal8Bit ("警告"),QString::fromLocal8Bit ("发送不能为空"));

    }
}

void MainWindow::on_textEdit_ReceiveData_textChanged()
{
    if(ui->checkBox_addReceiveTime->checkState())
    {
        QTime curTime = QTime::currentTime();
        dataTime = curTime.toString("hh:mm:ss.zzz");
    }
}

QString MainWindow::getCurrentTime()
{
    QString Time;
    if (ui->checkBox_addReceiveTime->checkState())
    {
        QTime curTime = QTime::currentTime();
        Time = curTime.toString("hh:mm:ss.zzz");
    }
    else{
        Time = "";
    }
    return Time;
}

void MainWindow::on_pushButton_Sendfile_clicked()
{
    QString filter = "ALL File(*.*);;Text File(*.txt);;XML File (*.xml)";
    QString filename = QFileDialog::getOpenFileName(this,"open a file","C://",filter);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        line = line + "\n";
        //   qDebug()<<line;
        ui->lineEdit_Sendedit->setText(line);
        emit (sendfile_line());
        ui->lineEdit_Sendedit->setText("");
    }

    file.close();

}

void MainWindow::on_checkBox_SendFileData_clicked(bool checked)
{
    if(checked){

        QMessageBox::information(this, "Tips",QString::fromLocal8Bit("敬请期待"),QMessageBox::Yes);
        ui->checkBox_SendFileData->setChecked(false);
    }

}
