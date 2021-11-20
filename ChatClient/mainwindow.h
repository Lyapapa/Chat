#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QListView>
#include <QStandardItemModel>
#include <QTimer>
#include <QInputDialog>

#include "tcppart.h"
#include "structs.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void makeGroupChatTitle(const QStringList& members, const int& tabIndex);
    void checkGroupChatMembers(const QStringList& members, const int& tabIndex);

private slots:
    void on_pushButtonAutoriz_clicked();

    void on_pushButtonAutorizoShow_clicked();

    void on_pushButtonAddChat_clicked();

    void hideAndShowComponents(const bool& newStatus);

    void on_pushButtonGroupChat_clicked();
    void autorizResult(const QString& loginRes, const QString& pasRes, const DbAnswers& autoRes, const QStringList& userList);
    void messageResult(const QString& sender, const QStringList& receivers, const QString& dateTime, const QString& text);
    void startWriteRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
    void mesReceiveRes(const QString& sender, const QStringList& receivers, const QString& dateTime);
    void historyRes(const QString& sender, const QList<chatHistoryMessage>& history);

    saveUniqueChat* createNewTab(const bool& isGroupChat);

    void on_pushButAddNew_clicked();

signals:
    void sendText(const QString& userName, const QDateTime& curDateTime, const QString& textForSend);
    void sendMessageForBuild(const QString& senderName, const QStringList& receiverName, const QString& messageText);

private:
    Ui::MainWindow *ui;
    TcpPart *m_tcpTransfer;

    QStringList m_allLogins;
    QStringList m_loginsForSingleChat;

    QString m_myLogin;
    QList<memPlaceForText> m_saveTabMap;
    QMap<int, QStringList> m_tabsIndexAndMembersMap;
    bool m_tryCreateLogin;

};
#endif // MAINWINDOW_H
