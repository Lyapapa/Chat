#include "tcppart.h"

TcpPart::TcpPart(QObject *parent)
{
    socket = new QTcpSocket(this);

    QObject::connect(this, &TcpPart::newMessage, this, &TcpPart::displayMessage);
    QObject::connect(socket, &QTcpSocket::readyRead, this, &TcpPart::readSocket);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &TcpPart::discardSocket);
    QObject::connect(socket, &QAbstractSocket::errorOccurred, this, &TcpPart::displayError);

    socket->connectToHost(QHostAddress::LocalHost, 1234);

    if(socket->waitForConnected())
         emit newMessage("Connected to Server");
    else
    {
        emit newMessage(QString("The following error occurred: %1.").arg(socket->errorString()));
        QMessageBox::critical(nullptr, "Нет соединения с сервером", "Попробуйте позже");
    }

    m_jsonConvector = new JsonConverter(this);
    QObject::connect(m_jsonConvector, &JsonConverter::autorizResult, this, &TcpPart::autorizResult);
    QObject::connect(m_jsonConvector, &JsonConverter::messageResult, this, &TcpPart::messageResult);
    QObject::connect(m_jsonConvector, &JsonConverter::startWriteRes, this, &TcpPart::startWriteRes);
    QObject::connect(m_jsonConvector, &JsonConverter::mesReceiveRes, this, &TcpPart::mesReceiveRes);
    QObject::connect(m_jsonConvector, &JsonConverter::historyRes, this, &TcpPart::historyRes);
}

void TcpPart::displayMessage(const QString& str)
{
    qDebug() << str;
}

void TcpPart::readSocket()
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

    socketStream.startTransaction();
    socketStream >> buffer;
    emit newMessage(buffer);
    m_jsonConvector->readJson(jsonByteArray);
}

void TcpPart::discardSocket()
{
    socket->deleteLater();
    socket=nullptr;

    emit newMessage("Disconnected");
}

void TcpPart::displayError(const QAbstractSocket::SocketError& socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            emit newMessage("The host was not found. Please check the host name and port settings.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            emit newMessage("The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
        default:
            emit newMessage(QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}

void TcpPart::on_pushButton_sendMessage_clicked()
{
    if(socket)
    {
        if(!socket->isOpen())
            emit newMessage("Socket doesn't seem to be opened");
    }
    else
        emit newMessage("Not connected");
}

void TcpPart::sendMessageToServer(const QString& senderName, const QStringList& receiverNames, const QString& messageText)
{
    QString dateTime = QDate::currentDate().toString(Qt::SystemLocaleShortDate) + " " + QTime::currentTime().toString(Qt::TextDate);
    QByteArray arrForSend = m_jsonConvector->writeJsonMessage(senderName, dateTime, receiverNames, messageText);
    sendToSocketData(arrForSend);
}

void TcpPart::sendTakeHistoryToServer(const QString& senderName, const QStringList& receiverNames)
{
    QByteArray arrForSend = m_jsonConvector->writeJsonHistory(senderName, receiverNames);
    sendToSocketData(arrForSend);
}

void TcpPart::sendNewLoginToServer(const QString& newLogin, const QString& newPassword)
{
    QByteArray arrForSend = m_jsonConvector->writeJsonNewLogin(newLogin, newPassword);
    sendToSocketData(arrForSend);
}

void TcpPart::sendAutorisationToServer(const QString& login, const QString& password)
{
    QByteArray arrForSend = m_jsonConvector->writeJsonAutoriz(login, password);
    sendToSocketData(arrForSend);
}

void TcpPart::sendReceiveMessageToServer(const QString &senderName, const QStringList& receiverNames)
{
    QString dateTime = QDate::currentDate().toString(Qt::SystemLocaleShortDate) + " " + QTime::currentTime().toString(Qt::TextDate);
    QByteArray arrForSend = m_jsonConvector->writeJsonReceive(senderName, dateTime, receiverNames);
    sendToSocketData(arrForSend);
}

void TcpPart::sendStartWriteToServer(const QString& senderName, const QStringList& receiverNames)
{
    QString dateTime = QDate::currentDate().toString(Qt::SystemLocaleShortDate) + " " + QTime::currentTime().toString(Qt::TextDate);
    QByteArray arrForSend = m_jsonConvector->writeJsonStartWr(senderName, dateTime, receiverNames);
    sendToSocketData(arrForSend);
}

void TcpPart::sendToSocketData(const QByteArray& byteArray)
{
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream << byteArray;
}
