#ifndef STRUCTS_H
#define STRUCTS_H

#include <QString>

struct chatHistoryMessage{
    QString sender;
    QString receivers;
    QString message;
    QString dateTime;
    QString chatMems;
    QString whoReceiveThis;
};

enum JsonMode{
    JM_AAUTORIZATION = 0, JM_MESSAGE, JM_STARWRITE, JM_MESSAGERECEIVE
};

enum DbAnswers{
    DBA_NOT_FOUND_LOGIN = 0, DBA_NOT_CURRENT_PASSWORD, DBA_ALL_GOOD, DBA_BEFORE_CHECK, DBA_UNKNOWN_STATE, DBA_LOG_BEFORE
};

#endif // STRUCTS_H
