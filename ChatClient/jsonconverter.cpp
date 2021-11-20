#include "jsonconverter.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <QFile>

JsonConverter::JsonConverter(QObject *parent)
{

}

QByteArray JsonConverter::writeJsonAutoriz(const QString& login, const QString& password)
{
    QJsonObject jsonObj;
    QByteArray byteArray;

    jsonObj["Mode"] = QString("autorisation");
    jsonObj["Login"] = login;
    jsonObj["Password"] = password;
    jsonObj["Result"] = DBA_BEFORE_CHECK;

    byteArray = QJsonDocument(jsonObj).toJson();
    return byteArray;
}

QByteArray JsonConverter::writeJsonNewLogin(const QString& newLogin, const QString& newPassword)
{
    QJsonObject jsonObj;
    QByteArray byteArray;

    jsonObj["Mode"] = QString("new_login");
    jsonObj["Login"] = newLogin;
    jsonObj["Password"] = newPassword;
    jsonObj["Result"] = DBA_BEFORE_CHECK;

    byteArray = QJsonDocument(jsonObj).toJson();
    return byteArray;
}

QByteArray JsonConverter::writeJsonMessage(const QString& sender, const QString& dateTime, const QStringList& receiverNames, const QString& text)
{
    QJsonObject jsonObj;
    QJsonArray receivers;
    QByteArray byteArray;

    jsonObj["Mode"] = QString("message");
    jsonObj["Sender"] = sender;
    receivers = QJsonArray::fromStringList(receiverNames);
    jsonObj["Receivers"] = receivers;
    jsonObj["QDateTime"] = dateTime;
    jsonObj["Text"] = text;

    byteArray = QJsonDocument(jsonObj).toJson();
    return byteArray;
}

QByteArray JsonConverter::writeJsonStartWr(const QString& sender, const QString& dateTime, const QStringList& receiverNames)
{
    QJsonObject jsonObj;
    QJsonArray receivers;
    QByteArray byteArray;

    jsonObj["Mode"] = QString("start_write");
    jsonObj["Sender"] = sender;
    receivers = QJsonArray::fromStringList(receiverNames);
    jsonObj["Receivers"] = receivers;
    jsonObj["QDateTime"] = dateTime;

    byteArray = QJsonDocument(jsonObj).toJson();
    return byteArray;
}

QByteArray JsonConverter::writeJsonReceive(const QString& sender, const QString& dateTime, const QStringList& receiverNames)
{
    QJsonObject jsonObj;
    QJsonArray receivers;
    QByteArray byteArray;

    jsonObj["Mode"] = QString("receive");
    jsonObj["Sender"] = sender;
    receivers = QJsonArray::fromStringList(receiverNames);
    jsonObj["Receivers"] = receivers;
    jsonObj["QDateTime"] = dateTime;

    byteArray = QJsonDocument(jsonObj).toJson();
    return byteArray;
}

QByteArray JsonConverter::writeJsonHistory(const QString& sender, const QStringList& receiverNames)
{
    QJsonObject jsonObj;
    QJsonArray receivers;
    QByteArray byteArray;

    jsonObj["Mode"] = QString("update_history");
    jsonObj["Sender"] = sender;
    receivers = QJsonArray::fromStringList(receiverNames);
    jsonObj["Receivers"] = receivers;

    byteArray = QJsonDocument(jsonObj).toJson();
    return byteArray;
}

void JsonConverter::readJson(const QByteArray& byteArray)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);
    if (parseError.error != QJsonParseError::NoError){
        qWarning() << "Parse error at " << parseError.offset << ":" << parseError.errorString();
        return;
    }

    QJsonObject jsonObj;
    jsonObj = jsonDoc.object();
    JsonMode mode;
    QString modeStr = jsonObj.value("Mode").toString();

    if (modeStr == "autorisation") readJsonAutoriz(jsonObj);
    else if (modeStr == "message") readJsonMessage(jsonObj);
    else if (modeStr == "start_write") readJsonStartWr(jsonObj);
    else if (modeStr == "receive") readJsonReceive(jsonObj);
    else if (modeStr == "update_history") readJsonHistory(jsonObj);
}

void JsonConverter::readJsonHistory(const QJsonObject& jsonObj)
{
    QList<chatHistoryMessage> history;
    QJsonValue jsonVal;
    QString sender = jsonObj.value("Sender").toString();
    QJsonArray arrJs = jsonObj.value("history").toArray();
    for (int i = 0; i < arrJs.size(); i++)
    {
        chatHistoryMessage oneMessage;
        oneMessage.sender = arrJs.at(i).toObject().value("messageSender").toString();
        oneMessage.receivers = arrJs.at(i).toObject().value("messageReceivers").toString();
        oneMessage.message = arrJs.at(i).toObject().value("messageText").toString();
        oneMessage.dateTime = arrJs.at(i).toObject().value("messageDateTime").toString();
        oneMessage.chatMems = arrJs.at(i).toObject().value("messagechatMems").toString();
        oneMessage.whoReceiveThis = arrJs.at(i).toObject().value("messagewhoReceiveThis").toString();

        history.append(oneMessage);
    }

    emit historyRes(sender, history);
}

void JsonConverter::readJsonAutoriz(const QJsonObject& jsonObj)
{
    QString loginRes = jsonObj.value("Login").toString();
    QString pasRes = jsonObj.value("Password").toString();
    DbAnswers autoRes = static_cast<DbAnswers>(jsonObj.value("Result").toInt());

    QJsonValue jsonVal;
    jsonVal = jsonObj.value("allUsers");
    QJsonArray arrJs = jsonVal.toArray();
    QStringList allUsers;
    for (int i = 0; i < arrJs.size(); i++)
    {
        allUsers.append(arrJs.at(i).toString());
    }

    emit autorizResult(loginRes, pasRes, autoRes, allUsers);
}

void JsonConverter::readJsonMessage(const QJsonObject& jsonObj)
{
    QJsonValue jsonVal;
    QString sender = jsonObj.value("Sender").toString();
    jsonVal = jsonObj.value("Receivers");
    QJsonArray arrJs = jsonVal.toArray();
    QStringList receivers;
    for (int i = 0; i < arrJs.size(); i++)
    {
        receivers.append(arrJs.at(i).toString());
    }
    QString dateTime = jsonObj.value("QDateTime").toString();
    QString text = jsonObj.value("Text").toString();

    emit messageResult(sender, receivers, dateTime, text);
}

void JsonConverter::readJsonStartWr(const QJsonObject& jsonObj)
{
    QJsonValue jsonVal;
    QString sender = jsonObj.value("Sender").toString();
    jsonVal = jsonObj.value("Receivers");
    QJsonArray arrJs = jsonVal.toArray();
    QStringList receivers;
    for (int i = 0; i < arrJs.size(); i++)
    {
        receivers.append(arrJs.at(i).toString());
    }
    QString dateTime = jsonObj.value("QDateTime").toString();

    emit startWriteRes(sender, receivers, dateTime);
}

void JsonConverter::readJsonReceive(const QJsonObject& jsonObj)
{
    QJsonValue jsonVal;
    QString sender = jsonObj.value("Sender").toString();
    jsonVal = jsonObj.value("Receivers");
    QJsonArray arrJs = jsonVal.toArray();
    QStringList receivers;
    for (int i = 0; i < arrJs.size(); i++)
    {
        receivers.append(arrJs.at(i).toString());
    }
    QString dateTime = jsonObj.value("QDateTime").toString();

    emit mesReceiveRes(sender, receivers, dateTime);
}
