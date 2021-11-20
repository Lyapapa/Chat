#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H

#include <QString>
#include <QObject>

#include "structs.h"

class JsonConverter : public QObject
{
    Q_OBJECT
public:
    JsonConverter(QObject *parent = nullptr);

    QByteArray writeJsonAutoriz(const QString& login, const QString& password, const DbAnswers& autoRes, const QStringList& userList);
    QByteArray writeJsonMessage(const QString& sender, const QString& dateTime, const QStringList& receiverNames, const QString& text);
    QByteArray writeJsonStartWr(const QString& sender, const QString& dateTime, const QStringList& receiverNames);
    QByteArray writeJsonReceive(const QString& sender, const QString& dateTime, const QStringList& receiverNames);
    QByteArray writeJsonHistory(const QString&  sender, const QList<chatHistoryMessage>& history);

    void readJson(const QByteArray& byteArray);
    void readJsonAutoriz(const QJsonObject& jsonObj);
    void readJsonNewLogin(const QJsonObject& jsonObj);
    void readJsonMessage(const QJsonObject& jsonObj);
    void readJsonStartWr(const QJsonObject& jsonObj);
    void readJsonReceive(const QJsonObject& jsonObj);
    void readJsonUpdateHistory(const QJsonObject& jsonObj);

signals:
    void newLoginCreate(const QString& loginRes, const QString& pasRes);
    void autorizCheck(const QString& loginRes, const QString& pasRes);
    void updateHistory(const QString& sender, const QStringList& receivers);
    void sendingMessage(const QString& sender, const QStringList& receivers, const QString& dateTime, const QString& text);

    void sendMessage();

    void autorizResult(const QString& loginRes, const QString& pasRes, const DbAnswers& autoRes, const QStringList& userList);
    void messageResult(const QString& sender, const QStringList& receivers, const QString& dateTime, const QString& text);
    void startWriteRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
    void mesReceiveRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
};

#endif // JSONCONVERTER_H
