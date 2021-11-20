#include "loginpasswordbd.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

const QString LoginPasswordBD::tableName = QString("LoginPassword");
const QString LoginPasswordBD::fieldLogin = QString("Login");
const QString LoginPasswordBD::fieldPassword = QString("Password");

LoginPasswordBD::LoginPasswordBD(QObject *parent, const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "Logins");
    m_db.setDatabaseName(path);
    m_db.open();
    m_needCreateSomeLogins = false;
    printAllPersons();

    if (!m_needCreateSomeLogins)
    {
        createSomeLogins();
    }
}

LoginPasswordBD::~LoginPasswordBD()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

void LoginPasswordBD::createSomeLogins()
{
    if (m_db.isOpen())
    {
        createTable();   // Creates a table if it doens't exist. Otherwise, it will use existing table.
        addPerson("user1","pas1");
        addPerson("user2","pas2");
        addPerson("user3","pas3");
        addPerson("user4","pas4");
        printAllPersons();
    }

}

bool LoginPasswordBD::isOpen() const
{
    return m_db.isOpen();
}

bool LoginPasswordBD::createTable()
{
    bool success = false;

    QSqlQuery query(m_db);
    query.prepare("CREATE TABLE " + tableName + "(id INTEGER PRIMARY KEY, " + fieldLogin + " TEXT NOT NULL UNIQUE, " + fieldPassword + " TEXT);");

    if (!query.exec())
    {
        success = false;
    }

    return success;
}

bool LoginPasswordBD::addPerson(const QString& log, const QString& pass)
{
    bool success = false;

    if (!log.isEmpty() || !pass.isEmpty())
    {
        QSqlQuery queryAdd(m_db);
        queryAdd.prepare("INSERT INTO " + tableName + " (" + fieldLogin + "," + fieldPassword + ")"
                "VALUES (:" + fieldLogin + ", :" + fieldPassword + ")");
        queryAdd.bindValue(":" + fieldLogin + "", log);
        queryAdd.bindValue(":" + fieldPassword + "", pass);


        if(queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "add person failed: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "add person failed: name cannot be empty";
    }

    return success;
}

void LoginPasswordBD::printAllPersons()
{
    bool isAnybodyHereWhere = false;
    QSqlQuery query("SELECT * FROM " + tableName, m_db);
    int idLog = query.record().indexOf(fieldLogin);
    int idPass = query.record().indexOf(fieldPassword);

    while (query.next())
    {
        QString log = query.value(idLog).toString();
        QString pass = query.value(idPass).toString();
        qDebug() << fieldLogin + " : " + log << fieldPassword + " : " + pass;
        isAnybodyHereWhere = true;
    }

    m_needCreateSomeLogins = isAnybodyHereWhere;
}

QStringList LoginPasswordBD::takeAllLogins() const
{
    QStringList loginList;
    QSqlQuery query("SELECT * FROM " + tableName, m_db);
    int idLog = query.record().indexOf(fieldLogin);
    int idPass = query.record().indexOf(fieldPassword);

    while (query.next())
    {
        loginList.append(query.value(idLog).toString());
    }
    return loginList;
}

void LoginPasswordBD::createNewLogin(const QString& login, const QString& password)
{
    DbAnswers checkRes = DBA_UNKNOWN_STATE;

    if (addPerson(login, password))
    {
        checkRes = DBA_ALL_GOOD;
    }
    QStringList userList = takeAllLogins();

    emit checkLoginResult(login, password, checkRes, userList);
}

void LoginPasswordBD::checkLoginPassword(const QString& login, const QString& password)
{
    QSqlQuery checkQuery("SELECT * FROM " + tableName + " WHERE " + fieldLogin + " = '" + login + "'", m_db);
    QSqlRecord rec = checkQuery.record();
    QStringList userList;

    DbAnswers checkRes;
    if (!rec.isEmpty())
    {
        qDebug() << "checkLoginPassword";
        int idPas = checkQuery.record().indexOf(fieldPassword);
        if (checkQuery.next())
        {
            QString passFromBd = checkQuery.value(idPas).toString();
            if (password == passFromBd)
            {
                userList = takeAllLogins();
                checkRes = DBA_ALL_GOOD;
            }
            else
            {
                checkRes = DBA_NOT_CURRENT_PASSWORD;
            }
        }
    }
    else
    {
        checkRes = DBA_NOT_FOUND_LOGIN;
    }
    emit checkLoginResult(login, password, checkRes, userList);

}

bool LoginPasswordBD::removeAllPersons()
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
