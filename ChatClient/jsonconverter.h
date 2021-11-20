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
    QByteArray writeJsonAutoriz(const QString& login, const QString& password);
    QByteArray writeJsonMessage(const QString& sender, const QString& dateTime, const QStringList& receiverNames, const QString& text);
    QByteArray writeJsonStartWr(const QString& sender, const QString& dateTime, const QStringList& receiverNames);
    QByteArray writeJsonReceive(const QString& sender, const QString& dateTime, const QStringList& receiverNames);
    QByteArray writeJsonHistory(const QString& sender, const QStringList& receiverNames);
    QByteArray writeJsonNewLogin(const QString& newLogin, const QString& newPassword);

    void readJson(const QByteArray& byteArray);
    void readJsonAutoriz(const QJsonObject& jsonObj);
    void readJsonMessage(const QJsonObject& jsonObj);
    void readJsonStartWr(const QJsonObject& jsonObj);
    void readJsonReceive(const QJsonObject& jsonObj);
    void readJsonHistory(const QJsonObject& jsonObj);

signals:
    void autorizResult(const QString& loginRes, const QString& pasRes, const DbAnswers& autoRes, const QStringList& userList);
    void messageResult(const QString& sender, const QStringList& receivers, const QString& dateTime, const QString& text);
    void startWriteRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
    void mesReceiveRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
    void historyRes(const QString& sender, const QList<chatHistoryMessage>& history);
};

#endif // JSONCONVERTER_H
