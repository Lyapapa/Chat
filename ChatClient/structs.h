#ifndef STRUCTS_H
#define STRUCTS_H

#include <QString>
#include <QTextEdit>
#include <QLabel>
#include <QStringList>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

struct chatHistoryMessage{
    QString sender;
    QString receivers;
    QString message;
    QString dateTime;
    QString chatMems;
    QString whoReceiveThis;
};

struct memPlaceForText{
    QTextEdit* curTexEdit;
    QLabel* statusLabel;
    QStringList listOfWriters;
    QString sender;
    QStringList receivers;
};

struct saveUniqueChat{
    QWidget* tab;
    QTextEdit* charHistory;
    QLabel* statusLabel;
    QPushButton* sendBut;
    QPushButton* updBut;
    QLineEdit* userText;
    QComboBox* userList;
    QStringList chatMembers;
    quint8 tabIndex;

};

enum JsonMode{
    JM_AAUTORIZATION = 0, JM_MESSAGE, JM_STARWRITE, JM_MESSAGERECEIVE
};

enum DbAnswers{
    DBA_NOT_FOUND_LOGIN = 0, DBA_NOT_CURRENT_PASSWORD, DBA_ALL_GOOD, DBA_BEFORE_CHECK, DBA_UNKNOWN_STATE, DBA_LOG_BEFORE
};

#endif // STRUCTS_H
