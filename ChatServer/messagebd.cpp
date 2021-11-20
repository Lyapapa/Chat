#include "messagebd.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

const QString MessagesBD::tableName = QString("ChatHistory");
const QString MessagesBD::fieldSender = QString("Sender");
const QString MessagesBD::fieldReceivers = QString("Receivers");
const QString MessagesBD::fieldMessage = QString("Message");
const QString MessagesBD::fieldDateTime = QString("DateTime");
const QString MessagesBD::fieldChatMembers = QString("ChatMembers");
const QString MessagesBD::fieldWhoReceive = QString("WhoReceive");

MessagesBD::MessagesBD(QObject *parent, const QString &path) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "MESSAGES");
    m_db.setDatabaseName(path);
    m_db.open();
}

MessagesBD::~MessagesBD()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

void MessagesBD::createSomeRecords()
{
    if (m_db.isOpen())
    {
        createTable();   // Creates a table if it doens't exist. Otherwise, it will use existing table.
        addNewRecord("user1","user2", "test1", "20.11.2021 11:34", "user1, user2", "user1, user2");
        addNewRecord("user1","user2", "test11", "20.11.2021 11:35", "user1, user2", "user1, user2");
        addNewRecord("user1","user3", "test2", "20.11.2021 11:35", "user1, user3", "user1, user3");
        addNewRecord("user1","user4", "test3", "20.11.2021 11:36", "user1, user4", "user1, user4");
        addNewRecord("user1","user5", "test4", "20.11.2021 11:37", "user1, user5", "user1, user5");
        addNewRecord("user1","user6", "test5", "20.11.2021 11:38", "user1, user6", "user1, user6");
    }

}

bool MessagesBD::isOpen() const
{
    return m_db.isOpen();
}

bool MessagesBD::createTable()
{
    bool success = false;

    QSqlQuery query(m_db);
    query.prepare("CREATE TABLE " + tableName + "(id INTEGER PRIMARY KEY, " + fieldSender + " TEXT NOT NULL, "
                  + fieldReceivers + " TEXT NOT NULL, " + fieldMessage + " TEXT NOT NULL, " + fieldDateTime + " TEXT NOT NULL, "
                  + fieldChatMembers + " TEXT NOT NULL, " + fieldWhoReceive + " TEXT);");

    if (!query.exec())
    {
        success = false;
    }

    return success;
}

bool MessagesBD::addNewRecord(const QString& sender, const QString& receivers, const QString& message,
                              const QString& dTime, const QString& chatMembers, const QString& whoReceive)
{
    bool success = false;

    if (!sender.isEmpty() || !receivers.isEmpty() || !message.isEmpty()
            || !dTime.isEmpty() || !chatMembers.isEmpty())
    {
        QSqlQuery queryAdd(m_db);
        queryAdd.prepare("INSERT INTO " + tableName + " (" + fieldSender + "," + fieldReceivers + "," + fieldMessage +
                         "," + fieldDateTime + "," + fieldChatMembers + "," + fieldWhoReceive +")"
                "VALUES (:" + fieldSender + ", :" + fieldReceivers + ", :" + fieldMessage + ", :" + fieldDateTime
                         + ", :" + fieldChatMembers + ", :" + fieldWhoReceive + ")");
        queryAdd.bindValue(":" + fieldSender + "", sender);
        queryAdd.bindValue(":" + fieldReceivers + "", receivers);
        queryAdd.bindValue(":" + fieldMessage + "", message);
        queryAdd.bindValue(":" + fieldDateTime + "", dTime);
        queryAdd.bindValue(":" + fieldChatMembers + "", chatMembers);
        queryAdd.bindValue(":" + fieldWhoReceive + "", whoReceive);


        if(queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "add addNewRecord failed: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "add addNewRecord failed: name cannot be empty";
    }

    return success;
}

void MessagesBD::historyOfChat(const QString& sender, const QStringList& receivers)
{
    QStringList listOfMembers;
    listOfMembers.append(sender);
    listOfMembers.append(receivers);
    qSort(listOfMembers);
    QList<chatHistoryMessage> history = findHistoryOfChat(listOfMembers.join(", "));
    emit takeHistoryOfChat(sender, receivers, history);
}

QList<chatHistoryMessage> MessagesBD::findHistoryOfChat(const QString& chatMembers)
{
    QList<chatHistoryMessage> chatHistory;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM " + tableName + " WHERE " + fieldChatMembers + " = '" + chatMembers + "'");

    while (query.next())
    {
        int idSender = query.record().indexOf(fieldSender);
        int idReceivers = query.record().indexOf(fieldReceivers);
        int idMessage = query.record().indexOf(fieldMessage);
        int idDt = query.record().indexOf(fieldDateTime);
        int idChatMems = query.record().indexOf(fieldChatMembers);
        int idWhoReceive = query.record().indexOf(fieldWhoReceive);

        chatHistoryMessage tempStruct;
        tempStruct.sender = query.value(idSender).toString();
        tempStruct.receivers = query.value(idReceivers).toString();
        tempStruct.message = query.value(idMessage).toString();
        tempStruct.dateTime = query.value(idDt).toString();
        tempStruct.chatMems = query.value(idChatMems).toString();
        tempStruct.whoReceiveThis = query.value(idWhoReceive).toString();
        qDebug() << fieldSender + " : " + tempStruct.sender << fieldReceivers + " : " + tempStruct.receivers
                    << fieldMessage + " : " + tempStruct.message << fieldDateTime + " : " + tempStruct.dateTime
                    << fieldChatMembers + " : " + tempStruct.chatMems
                    << fieldWhoReceive + " : " + tempStruct.whoReceiveThis;

        chatHistory.append(tempStruct);
    }
    return chatHistory;
}

void MessagesBD::changeValueWhoReceiveInBd(const QString& chatMembers,
                     const QString& dt, const QString& whoReceived)
{
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM " + tableName + " WHERE " + fieldChatMembers + " = '" + chatMembers + "'" +
               " AND " + fieldDateTime + " = '" + dt + "'");

    if (query.next())
    {
        int idSender = query.record().indexOf(fieldSender);
        int idReceivers = query.record().indexOf(fieldReceivers);
        int idMessage = query.record().indexOf(fieldMessage);
        int idDt = query.record().indexOf(fieldDateTime);
        int idChatMems = query.record().indexOf(fieldChatMembers);
        int idWhoReceive = query.record().indexOf(fieldWhoReceive);


        QString sender = query.value(idSender).toString();
        QString receivers = query.value(idReceivers).toString();
        QString message = query.value(idMessage).toString();
        QString dateTime = query.value(idDt).toString();
        QString chatMems = query.value(idChatMems).toString();
        QString whoReceive = query.value(idWhoReceive).toString();
        qDebug() << fieldSender + " : " + sender << fieldReceivers + " : " + receivers
                    << fieldMessage + " : " + message << fieldDateTime + " : " + dateTime
                    << fieldChatMembers + " : " + chatMems << fieldWhoReceive + " : " + whoReceive;


        QStringList listTemp = whoReceive.split(", ");
        if (listTemp.size() == 1)
        {
            if (whoReceive == whoReceived)
            {
                if (!whoReceive.contains("!"))
                {
                    listTemp.clear();
                    listTemp.append(whoReceive.append("!"));
                }
            }
        }
        else
        {
            for (int i = 0; i < listTemp.size(); i++)
            {
                if (listTemp[i] == whoReceived)
                {
                    qDebug() << listTemp[i] << whoReceived;
                    if (!listTemp[i].contains("!"))
                    {
                        listTemp[i].append("!");
                    }
                }
            }
        }

        QString newStr = listTemp.join(", ");
        query.exec("UPDATE " + tableName + " SET " + fieldWhoReceive + " = '" + newStr + "'"
                   + " WHERE " + fieldChatMembers + " = '" + chatMembers + "'" +
                   " AND " + fieldDateTime + " = '" + dt + "'");
    }
}

void MessagesBD::printAllRecords() const
{
    QSqlQuery query("SELECT * FROM " + tableName, m_db);
    int idSender = query.record().indexOf(fieldSender);
    int idReceivers = query.record().indexOf(fieldReceivers);
    int idMessage = query.record().indexOf(fieldMessage);
    int idDt = query.record().indexOf(fieldDateTime);
    int idChatMems = query.record().indexOf(fieldChatMembers);
    int idWhoReceive = query.record().indexOf(fieldWhoReceive);

    while (query.next())
    {
        QString sender = query.value(idSender).toString();
        QString receivers = query.value(idReceivers).toString();
        QString message = query.value(idMessage).toString();
        QString dateTime = query.value(idDt).toString();
        QString chatMems = query.value(idChatMems).toString();
        QString whoReceive = query.value(idWhoReceive).toString();
        qDebug() << fieldSender + " : " + sender << fieldReceivers + " : " + receivers
                    << fieldMessage + " : " + message << fieldDateTime + " : " + dateTime
                    << fieldChatMembers + " : " + chatMems << fieldWhoReceive + " : " + whoReceive;
    }
}

bool MessagesBD::removeAllRecords()
{
    bool success = false;

    QSqlQuery removeQuery(m_db);
    removeQuery.prepare("DELETE FROM " + tableName);

    if (removeQuery.exec())
    {
        success = true;
    }

    QSqlDatabase::removeDatabase(tableName);
    return success;
}
