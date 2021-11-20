#include "jsonconverter.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <QFile>

#include "messagebd.h"

JsonConverter::JsonConverter(QObject *parent)
{

}

QByteArray JsonConverter::writeJsonHistory(const QString& sender, const QList<chatHistoryMessage>& history)
{
    QJsonObject jsonObj;
    QByteArray byteArray;
    QJsonArray allChatHistory;

    jsonObj["Mode"] = QString("update_history");
    jsonObj["Sender"] = sender;
    jsonObj["Receivers"] = sender;

    for (int i = 0; i < history.size();i++)
    {
        QJsonObject jsOb = {{"messageSender", history[i].sender},
                           {"messageReceivers", history[i].receivers},
                           {"messageText", history[i].message},
                           {"messageDateTime", history[i].dateTime},
                           {"messagechatMems", history[i].chatMems},
                           {"messagewhoReceiveThis", history[i].whoReceiveThis}};
        allChatHistory.append(jsOb);
    }
    jsonObj["history"] = allChatHistory;

    byteArray = QJsonDocument(jsonObj).toJson();
    return byteArray;
}

QByteArray JsonConverter::writeJsonAutoriz(const QString& login, const QString& password, const DbAnswers& autoRes, const QStringList& userList)
{
    QJsonObject jsonObj;
    QByteArray byteArray;
    QJsonArray allUsers;

    jsonObj["Mode"] = QString("autorisation");
    jsonObj["Login"] = login;
    jsonObj["Password"] = password;
    jsonObj["Result"] = autoRes;
    allUsers = QJsonArray::fromStringList(userList);
    jsonObj["allUsers"] = allUsers;

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

void JsonConverter::readJson(const QByteArray& byteArray)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qWarning() << "Parse error at " << parseError.offset << ":" << parseError.errorString();
        return;
    }

    QJsonObject jsonObj;
    jsonObj = jsonDoc.object();

    JsonMode mode;
    QString modeStr = jsonObj.value("Mode").toString();
    qDebug() << "readJson" << modeStr;
    if (modeStr == "autorisation") readJsonAutoriz(jsonObj);
    else if (modeStr == "message") readJsonMessage(jsonObj);
    else if (modeStr == "start_write") readJsonStartWr(jsonObj);
    else if (modeStr == "receive") readJsonReceive(jsonObj);
    else if (modeStr == "update_history") readJsonUpdateHistory(jsonObj);
    else if (modeStr == "new_login") readJsonNewLogin(jsonObj);
}

void JsonConverter::readJsonNewLogin(const QJsonObject& jsonObj)
{
    QString loginRes = jsonObj.value("Login").toString();
    QString pasRes = jsonObj.value("Password").toString();

    emit newLoginCreate(loginRes, pasRes);
}

void JsonConverter::readJsonAutoriz(const QJsonObject& jsonObj)
{
    QString loginRes = jsonObj.value("Login").toString();
    QString pasRes = jsonObj.value("Password").toString();

    emit autorizCheck(loginRes, pasRes);
}

void JsonConverter::readJsonUpdateHistory(const QJsonObject& jsonObj)
{
    qDebug() << "readJsonUpdateHistory";
    QJsonValue jsonVal;
    QString sender = jsonObj.value("Sender").toString();
    jsonVal = jsonObj.value("Receivers");
    QJsonArray arrJs = jsonVal.toArray();
    QStringList receivers;
    for (int i = 0; i < arrJs.size(); i++)
    {
        receivers.append(arrJs.at(i).toString());
    }
    emit updateHistory(sender, receivers);
}

void JsonConverter::readJsonMessage(const QJsonObject &jsonObj)
{
    qDebug() << "readJsonMessage";
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
    qDebug() << "before sendingMessage";
    emit sendingMessage(sender, receivers, dateTime, text);
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

