#ifndef TCPPART_H
#define TCPPART_H

#include <QAbstractSocket>
#include <QFile>
#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QMetaType>
#include <QString>
#include <QStandardPaths>
#include <QTcpSocket>
#include <QMessageBox>
#include <QObject>
#include <QMessageBox>
#include <QDateTime>

#include "jsonconverter.h"
#include "structs.h"

class TcpPart : public QObject
{
    Q_OBJECT
public:
    TcpPart(QObject *parent = nullptr);
    void sendToSocketData(const QByteArray& byteArray);

public:
    void sendMessageToServer(const QString& senderName, const QStringList& receiverNames, const QString& messageText);
    void sendAutorisationToServer(const QString& login, const QString& password);
    void sendReceiveMessageToServer(const QString& senderName, const QStringList& receiverNames);
    void sendStartWriteToServer(const QString& senderName, const QStringList& receiverNames);
    void sendTakeHistoryToServer(const QString& senderName, const QStringList& receiverNames);
    void sendNewLoginToServer(const QString& newLogin, const QString& newPassword);


signals:
    void newMessage(const QString&);
    void autorizResult(const QString& loginRes, const QString& pasRes, const DbAnswers& autoRes, const QStringList& userList);
    void messageResult(const QString& sender, const QStringList& receivers, const QString& dateTime, const QString& text);
    void startWriteRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
    void mesReceiveRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
    void historyRes(const QString& sender, const QList<chatHistoryMessage>& history);

private slots:
    void readSocket();
    void discardSocket();
    void displayError(const QAbstractSocket::SocketError& socketError);
    void displayMessage(const QString& str);

    void on_pushButton_sendMessage_clicked();

private:
    QTcpSocket* socket;
    JsonConverter *m_jsonConvector;
};

#endif // TCPPART_H
