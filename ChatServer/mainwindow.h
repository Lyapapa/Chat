#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextCodec>
#include <QTimer>
#include <QThreadPool>

#include "jsonconverter.h"
#include "loginpasswordbd.h"
#include "messagebd.h"
#include "structs.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void makeSpamForClients(const QStringList& receivers, const QByteArray& forSend);

signals:
    void newMessage(const QString& debugMessage);
private slots:
    void newConnection();
    void appendToSocketList(QTcpSocket* socket);

    void readSocket();
    void discardSocket();
    void displayError(const QAbstractSocket::SocketError& socketError);

    void displayMessage(const QString& str);
    void sendData(const QByteArray& data, const quint32& socketNumber);
    void sendMessage(QTcpSocket* socket, const QByteArray& data);
private:
    Ui::MainWindow *ui;

    QTcpServer* m_server;
    QSet<QTcpSocket*> connection_set;
    JsonConverter *m_jsonConvector;
    LoginPasswordBD *m_loginDb;
    MessagesBD *m_mesDb;

    QMap<QString,int> m_logins_sockets;
    quint32 m_curSocket;
};

#endif // MAINWINDOW_H
