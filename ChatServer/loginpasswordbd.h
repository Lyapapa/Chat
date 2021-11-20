#ifndef LOGINPASSWORDBD_H
#define LOGINPASSWORDBD_H

#include <QSqlDatabase>
#include <QObject>

#include "jsonconverter.h"

class LoginPasswordBD : public QObject
{
    Q_OBJECT
public:    
    LoginPasswordBD(QObject *parent, const QString& path);
    ~LoginPasswordBD();
    bool isOpen() const;
    bool createTable();
    bool addPerson(const QString& log, const QString& pass);
    void printAllPersons();
    QStringList takeAllLogins() const;
    bool removeAllPersons();
    void createSomeLogins();

public slots:
    void checkLoginPassword(const QString& login, const QString& password);
    void createNewLogin(const QString& login, const QString& password);

signals:
    void checkLoginResult(const QString& loginRes, const QString& pasRes, const DbAnswers& autoRes, const QStringList& userList);

private:
    QSqlDatabase m_db;
    bool m_needCreateSomeLogins;

    const static QString tableName;
    const static QString fieldLogin;
    const static QString fieldPassword;
};

#endif // LOGINPASSWORDBD_H
