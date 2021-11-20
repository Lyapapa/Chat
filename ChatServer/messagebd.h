#ifndef MessagesBD_H
#define MessagesBD_H

#include <QSqlDatabase>
#include <QObject>

#include "structs.h"

class MessagesBD : public QObject
{
    Q_OBJECT
public:
    explicit MessagesBD(QObject *parent = nullptr, const QString &path = "chatHistory.bd");
    ~MessagesBD();
    bool isOpen() const;
    bool createTable();
    bool addNewRecord(const QString& sender, const QString& receivers, const QString& message,
                      const QString& dTime, const QString& chatMembers, const QString& whoReceive);
    void printAllRecords() const;
    bool removeAllRecords();
    void createSomeRecords();
    QList<chatHistoryMessage> findHistoryOfChat(const QString& chatMembers);
    void changeValueWhoReceiveInBd(const QString& chatMembers,
                         const QString& dt, const QString& whoReceived);

public slots:
    void historyOfChat(const QString& sender, const QStringList& receivers);

signals:
    void takeHistoryOfChat(const QString& sender, const QStringList& receivers, const QList<chatHistoryMessage>& history);

private:
    QSqlDatabase m_db;

    const static QString tableName;
    const static QString fieldSender;
    const static QString fieldReceivers;
    const static QString fieldMessage;
    const static QString fieldDateTime;
    const static QString fieldChatMembers;
    const static QString fieldWhoReceive;
};

#endif // MessagesBD_H
