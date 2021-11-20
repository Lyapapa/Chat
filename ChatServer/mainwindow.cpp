#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = new QTcpServer();
    setWindowIcon(QIcon(":/server.png"));

    if(m_server->listen(QHostAddress::Any, 1234))
    {
       connect(this, &MainWindow::newMessage, this, &MainWindow::displayMessage);
       connect(m_server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
       ui->statusBar->showMessage("Server is listening...");
    }
    else
    {
        QMessageBox::critical(this,"QTCPServer",QString("Unable to start the server: %1.").arg(m_server->errorString()));
        exit(EXIT_FAILURE);
    }

    m_jsonConvector = new JsonConverter(this);

    m_loginDb = new LoginPasswordBD(this, "logins.db");

    m_mesDb = new MessagesBD(this, "messageHistory.db");

    QObject::connect(m_jsonConvector, &JsonConverter::autorizCheck, m_loginDb, &LoginPasswordBD::checkLoginPassword);
    QObject::connect(m_jsonConvector, &JsonConverter::newLoginCreate, m_loginDb, &LoginPasswordBD::createNewLogin);
    QObject::connect(m_loginDb, &LoginPasswordBD::checkLoginResult, [this](QString loginRes, QString pasRes, DbAnswers autoRes, QStringList userList){
        if (autoRes == DBA_ALL_GOOD)
        {
            if(m_logins_sockets.value(loginRes, -1) == -1)
            {
                displayMessage(QString("INFO :: Client with login:%1 has just entered the room").arg(loginRes));
                m_logins_sockets[loginRes] = m_curSocket;
            }
            else
            {
                autoRes = DBA_LOG_BEFORE;
            }

            qDebug() << m_logins_sockets;
        }
        QByteArray forSend = m_jsonConvector->writeJsonAutoriz(loginRes, pasRes, autoRes, userList);
        sendData(forSend, m_curSocket);
    });

    QObject::connect(m_jsonConvector, &JsonConverter::sendingMessage, [this](QString sender, QStringList receivers, QString dateTime, QString text){
        QStringList chatMembers;
        chatMembers.append(sender);
        chatMembers.append(receivers);
        qSort(chatMembers);

        m_mesDb->addNewRecord(sender, receivers.join(", "), text, dateTime, chatMembers.join(", "), chatMembers.join(", "));
        m_mesDb->printAllRecords();

        QByteArray forSend = m_jsonConvector->writeJsonMessage(sender, dateTime, receivers, text);

        makeSpamForClients(receivers, forSend);
    });

    QObject::connect(m_jsonConvector, &JsonConverter::startWriteRes, [this](QString sender, QStringList receivers, QString dateTime){
        QByteArray forSend = m_jsonConvector->writeJsonStartWr(sender, dateTime, receivers);
        qDebug() << "forSend" << forSend;
        makeSpamForClients(receivers, forSend);
    });

    QObject::connect(m_jsonConvector, &JsonConverter::mesReceiveRes, [this](QString sender, QStringList receivers, QString dateTime){
        QByteArray forSend = m_jsonConvector->writeJsonReceive(sender, dateTime, receivers);

        QStringList chatMembers;
        chatMembers.append(sender);
        chatMembers.append(receivers);
        qSort(chatMembers);
        m_mesDb->changeValueWhoReceiveInBd(chatMembers.join(", "), dateTime, sender);
        makeSpamForClients(receivers, forSend);
    });

    QObject::connect(m_jsonConvector, &JsonConverter::updateHistory, m_mesDb, &MessagesBD::historyOfChat);
    QObject::connect(m_mesDb, &MessagesBD::takeHistoryOfChat,[this](QString sender, QStringList receivers, QList<chatHistoryMessage> history){
        QByteArray forSend = m_jsonConvector->writeJsonHistory(sender, history);
        makeSpamForClients(QStringList(sender), forSend);
    });
}

void MainWindow::makeSpamForClients(const QStringList& receivers, const QByteArray& forSend)
{
    for (int i = 0; i < receivers.size(); i++)
    {
        qint32 socketNum = m_logins_sockets.value(receivers.at(i), -1);
        displayMessage("TRY SEND TO " + QString::number(socketNum) + " == " + receivers.at(i));

        if(socketNum != -1)
        {
            displayMessage("SEND TO " + QString::number(socketNum) + " == " + receivers.at(i));

            QTimer::singleShot(10*i, [this, forSend, socketNum]() {
                sendData(forSend, socketNum);
            } );

        }
    }
}

MainWindow::~MainWindow()
{
    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    m_server->close();
    m_server->deleteLater();

    delete ui;
}

void MainWindow::newConnection()
{
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());
}

void MainWindow::appendToSocketList(QTcpSocket* socket)
{
    QThread* thread = new QThread();
    socket->moveToThread(thread);
    quint32 socketNum = socket->socketDescriptor();

    connection_set.insert(socket);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);
    QObject::connect(socket, &QTcpSocket::disconnected, [this, socketNum] (){
        qDebug() << m_logins_sockets.key(socketNum);
        displayMessage(QString("INFO :: A client with login:%1 has just left the room").arg(m_logins_sockets.key(socketNum)));
        m_logins_sockets.remove(m_logins_sockets.key(socketNum));
        qDebug() << m_logins_sockets;
    });
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);
    ui->comboBox_receiver->addItem(QString::number(socket->socketDescriptor()));
}

void MainWindow::readSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QByteArray buffer;

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);

    QByteArray byteArray = socket->readAll();

    quint8 needPlace = 0;
    for (int i = 0; i < byteArray.size();i++)
    {
        if (byteArray[i] == '{')
        {
            needPlace = i;
            break;
        }
    }
    QByteArray jsonByteArray = byteArray.mid(needPlace);

    emit newMessage(jsonByteArray);

    m_curSocket = socket->socketDescriptor();
    m_jsonConvector->readJson(jsonByteArray);

}

void MainWindow::discardSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QSet<QTcpSocket*>::iterator it = connection_set.find(socket);
    qDebug() << "connection_set" << connection_set;
    if (it != connection_set.end()){
        displayMessage(QString("INFO :: A client has just left the room").arg(socket->socketDescriptor()));
        connection_set.remove(*it);
    }
    
    socket->deleteLater();
}

void MainWindow::displayError(const QAbstractSocket::SocketError& socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, "QTCPServer", "The host was not found. Please check the host name and port settings.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, "QTCPServer", "The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
        default:
            QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
            QMessageBox::information(this, "QTCPServer", QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}

void MainWindow::sendData(const QByteArray& data, const quint32& socketNumber)
{
    foreach (QTcpSocket* socket,connection_set)
    {
        if (socket->socketDescriptor() == socketNumber)
        {
            sendMessage(socket, data);
        }
    }
}

void MainWindow::sendMessage(QTcpSocket* socket, const QByteArray& data)
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_15);
            socketStream.setVersion(QDataStream::Qt_5_15);
            socketStream << data;

            QString temp = data;
            QString temp2 = QString::number(socket->socketDescriptor());
            displayMessage(temp);
            displayMessage("socket num " + temp2);
        }
        else
            QMessageBox::critical(this,"QTCPServer","Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this,"QTCPServer","Not connected");
}


void MainWindow::displayMessage(const QString& str)
{
    ui->textBrowser_receivedMessages->append(str);
}

