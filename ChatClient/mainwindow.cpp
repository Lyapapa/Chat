#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_tryCreateLogin = false;
    hideAndShowComponents(false);
    setWindowIcon(QIcon(":/chat.png"));
    ui->pushButtonAddChat->setIcon(QIcon(":/add.png"));
    ui->pushButtonAutorizoShow->setIcon(QIcon(":/login.png"));
    ui->pushButtonGroupChat->setIcon(QIcon(":/chat (1).png"));
    ui->pushButAddNew->setIcon(QIcon(":/add.png"));

    m_tcpTransfer = new TcpPart(this);

    QObject::connect(m_tcpTransfer, &TcpPart::autorizResult, this, &MainWindow::autorizResult);
    QObject::connect(m_tcpTransfer, &TcpPart::messageResult, this, &MainWindow::messageResult);
    QObject::connect(m_tcpTransfer, &TcpPart::startWriteRes, this, &MainWindow::startWriteRes);
    QObject::connect(m_tcpTransfer, &TcpPart::mesReceiveRes, this, &MainWindow::mesReceiveRes);
    QObject::connect(m_tcpTransfer, &TcpPart::historyRes, this, &MainWindow::historyRes);

    ui->statusbar->showMessage("По умолчанию: 'user1 - pas1', ... 'user4' - 'pas4'");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startWriteRes(const QString& sender, const QStringList& receivers, const QString& dateTime)
{
    QStringList chatGroupInTab;
    QStringList chatGroupFromServer;

    chatGroupFromServer.append(sender);
    chatGroupFromServer.append(receivers);

    for (int i = 0; i < m_saveTabMap.size(); i++)
    {
        chatGroupInTab.clear();
        chatGroupInTab.append(m_saveTabMap[i].receivers);
        chatGroupInTab.append(m_saveTabMap[i].sender);
        qSort(chatGroupInTab);
        qSort(chatGroupFromServer);

        if (chatGroupInTab == chatGroupFromServer)
        {
            m_saveTabMap[i].listOfWriters.append(sender);
            m_saveTabMap[i].listOfWriters.removeDuplicates();
            if (m_saveTabMap[i].listOfWriters.size() == 1)
            {
                m_saveTabMap[i].statusLabel->setText(sender + " набирает сообщение...");
            }
            else
            {
                QString writers = m_saveTabMap[i].listOfWriters.join(",");
                m_saveTabMap[i].statusLabel->setText(writers + " набирают сообщение...");
            }

        }
    }
}

void MainWindow::historyRes(const QString& sender, const QList<chatHistoryMessage>& history)
{
    QStringList chatGroupFromServer;

    if (history.size() > 0)
    {
        chatGroupFromServer.append(history[0].chatMems.split(", "));
    }

    QStringList chatGroupInTab;
    int findPlace = -1;
    for (int i = 0; i < m_saveTabMap.size(); i++)
    {
        chatGroupInTab.clear();
        chatGroupInTab.append(m_saveTabMap[i].receivers);
        chatGroupInTab.append(m_saveTabMap[i].sender);
        qSort(chatGroupInTab);
        qSort(chatGroupFromServer);

        if (chatGroupInTab == chatGroupFromServer)
        {
            findPlace = i;
            break;
        }
    }

    if (findPlace != -1)
    {
        m_saveTabMap[findPlace].curTexEdit->clear();

        for (int i = 0; i < history.size(); i++)
        {
            bool mesAppend = false;
            if (history[i].sender == m_myLogin)
            {
                m_saveTabMap[findPlace].curTexEdit->setTextColor(Qt::darkGreen);
            }
            else
            {
                m_saveTabMap[findPlace].curTexEdit->setTextColor(Qt::blue);
                int indexOfReceive = history[i].whoReceiveThis.indexOf(m_myLogin) + history[i].sender.size();
                if (history[i].whoReceiveThis.size() > indexOfReceive)
                {
                    if (history[i].whoReceiveThis[indexOfReceive] == "!")
                    {

                        mesAppend = true;
                    }
                }

            }
            if (mesAppend)
            {
                m_saveTabMap[findPlace].curTexEdit->append(history[i].dateTime + " " + history[i].sender + " : "+ history[i].message + " ✓");
            }
            else
            {
                m_saveTabMap[findPlace].curTexEdit->append(history[i].dateTime + " " + history[i].sender + " : "+ history[i].message);
            }

        }
    }


}

void MainWindow::mesReceiveRes(const QString& sender, const QStringList& receivers, const QString& dateTime)
{
    QStringList chatGroupInTab;
    QStringList chatGroupFromServer;

    chatGroupFromServer.append(sender);
    chatGroupFromServer.append(receivers);

    for (int i = 0; i < m_saveTabMap.size(); i++)
    {
        chatGroupInTab.clear();
        chatGroupInTab.append(m_saveTabMap[i].receivers);
        chatGroupInTab.append(m_saveTabMap[i].sender);
        qSort(chatGroupInTab);
        qSort(chatGroupFromServer);

        if (chatGroupInTab == chatGroupFromServer)
        {
            QString allText = m_saveTabMap[i].curTexEdit->toPlainText();
            QStringList stringsList = allText.split('\n');

            for (int i2 = 0; i2 < stringsList.size(); i2++)
            {
                if (stringsList[i2].contains(dateTime + " " + m_myLogin + " : ") && !stringsList[i2].contains("(Доставлено)"))
                {
                    stringsList[i2].append(" ✓");
                    m_saveTabMap[i].curTexEdit->setPlainText("");

                    for (int i3 = 0; i3 < stringsList.size(); i3++)
                    {
                        if (stringsList[i3].indexOf(m_myLogin) == QString(dateTime + " ").size())
                        {
                            m_saveTabMap[i].curTexEdit->setTextColor(Qt::darkGreen);
                        }
                        else
                        {
                            m_saveTabMap[i].curTexEdit->setTextColor(Qt::blue);
                        }
                        m_saveTabMap[i].curTexEdit->append(stringsList[i3]);
                    }
                    break;
                }
            }

        }
    }
}

void MainWindow::messageResult(const QString& sender, const QStringList& receivers, const QString& dateTime, const QString& text)
{
    QStringList chatGroupInTab;
    QStringList chatGroupFromServer;

    chatGroupFromServer.append(sender);
    chatGroupFromServer.append(receivers);

    for (int i = 0; i < m_saveTabMap.size(); i++)
    {
        chatGroupInTab.clear();
        chatGroupInTab.append(m_saveTabMap[i].receivers);
        chatGroupInTab.append(m_saveTabMap[i].sender);
        qSort(chatGroupInTab);
        qSort(chatGroupFromServer);
        if (chatGroupInTab == chatGroupFromServer)
        {
            m_saveTabMap[i].curTexEdit->setTextColor(Qt::blue);
            m_saveTabMap[i].curTexEdit->append(dateTime + " " + sender + " : "+ text);
            QStringList newReceivers = receivers;
            newReceivers.append(sender);
            newReceivers.removeAll(m_myLogin);
            m_tcpTransfer->sendReceiveMessageToServer(m_myLogin, newReceivers);

            if (m_saveTabMap[i].statusLabel->text().contains(sender))
            {
                m_saveTabMap[i].listOfWriters.removeAll(sender);
                if (m_saveTabMap[i].listOfWriters.size() == 0)
                {
                    m_saveTabMap[i].statusLabel->setText("");
                }
                else if (m_saveTabMap[i].listOfWriters.size() == 1)
                {
                    m_saveTabMap[i].statusLabel->setText(m_saveTabMap[i].listOfWriters[0] + " набирает сообщение...");
                }
                else
                {
                    QString writers = m_saveTabMap[i].listOfWriters.join(",");
                    m_saveTabMap[i].statusLabel->setText(writers + " набирают сообщение...");
                }
            }
        }
    }
}

void MainWindow::autorizResult(const QString& loginRes, const QString& pasRes, const DbAnswers& autoRes, const QStringList& userList)
{
    if (autoRes == DBA_ALL_GOOD)
    {
        m_allLogins = userList;
        m_myLogin = loginRes;
        m_allLogins.removeAll(m_myLogin);
        m_loginsForSingleChat = m_allLogins;
        hideAndShowComponents(true);
        setWindowTitle("Приложение для обмена сообщениями (" + m_myLogin + ")");
        ui->statusbar->showMessage("");
    }
    else if (autoRes == DBA_NOT_FOUND_LOGIN)
    {
        QMessageBox::critical(this, "Внимание", "Пользователь не найден");
    }
    else if (autoRes == DBA_NOT_CURRENT_PASSWORD)
    {
        QMessageBox::critical(this, "Внимание", "Пароль не правильный");
    }
    else if (autoRes == DBA_LOG_BEFORE)
    {
        QMessageBox::critical(this, "Внимание", "Под данным логином уже произведено подключение");
    }
    else if (autoRes == DBA_UNKNOWN_STATE)
    {
        QMessageBox::critical(this, "Внимание", "Данный логин уже существует");
    }

    if (!m_tryCreateLogin)
    {
        m_tryCreateLogin = false;
    }
}

void MainWindow::hideAndShowComponents(const bool& newStatus)
{
    ui->groupBoxAutoriz->setHidden(newStatus);

    ui->pushButtonGroupChat->setHidden(!newStatus);
    ui->groupBoxChats->setHidden(!newStatus);
    ui->pushButtonAddChat->setHidden(!newStatus);
    ui->pushButtonAutorizoShow->setHidden(!newStatus);
}

void MainWindow::on_pushButtonAutoriz_clicked()
{
    if (!m_myLogin.isEmpty())
    {
        if (m_myLogin != ui->lineEditLogin->text())
        {
            for (int i = 0; i < ui->tabWidget->count(); i++)
            {
                ui->tabWidget->removeTab(i);
            }
        }
    }

    if (ui->lineEditLogin->text().isEmpty() || ui->lineEditPass->text().isEmpty())
    {
        QMessageBox::critical(this, "Внимание", "Логин/Пароль не были введены");
    }
    else
    {
        m_tcpTransfer->sendAutorisationToServer(ui->lineEditLogin->text(), ui->lineEditPass->text());
    }
}


void MainWindow::on_pushButtonAutorizoShow_clicked()
{
    hideAndShowComponents(false);
}

saveUniqueChat* MainWindow::createNewTab(const bool& isGroupChat)
{
    QWidget *newTab = new QWidget(this);
    QComboBox *newUsersList = new QComboBox(newTab);

    QLabel *newStatusLabel = new QLabel(newTab);

    quint8 newIndex = ui->tabWidget->count();

    QTextEdit *newChatHistory = new QTextEdit(newTab);
    newChatHistory->setReadOnly(true);

    QLineEdit *newUserText = new QLineEdit(newTab);

    QPushButton *newSendBut = new QPushButton(newTab);
    newSendBut->setText("Отправить сообщение");

    QPushButton *newUpdateHistory = new QPushButton(newTab);
    newUpdateHistory->setText("Обновить историю");
    newUpdateHistory->setIcon(QIcon(":/refresh.png"));

    QGridLayout *newLayout = new QGridLayout(newTab);
    newLayout->addWidget(newUsersList, 0, 1);
    newLayout->addWidget(newStatusLabel, 0, 0);
    newLayout->addWidget(newUpdateHistory, 0, 2);
    newLayout->addWidget(newChatHistory, 1, 0, 1, 3);
    newLayout->addWidget(newUserText, 2, 0, 1, 3);
    newLayout->addWidget(newSendBut, 3, 0, 1, 3);
    newTab->setLayout(newLayout);

    if(!isGroupChat)
    {
        ui->tabWidget->addTab(newTab, "Новый чат");
        newUsersList->addItems(m_loginsForSingleChat);
    }
    else
    {
        ui->tabWidget->addTab(newTab, "Новый групповой чат");
        newUsersList->addItems(m_allLogins);
    }
    newUsersList->setCurrentIndex(-1);
    newChatHistory->setEnabled(false);
    newUserText->setEnabled(false);
    newSendBut->setEnabled(false);
    saveUniqueChat* returnTab = new saveUniqueChat;
    returnTab->charHistory = newChatHistory;
    returnTab->sendBut = newSendBut;
    returnTab->tabIndex = newIndex;
    returnTab->userText = newUserText;
    returnTab->userList = newUsersList;
    returnTab->tab = newTab;
    returnTab->statusLabel = newStatusLabel;
    returnTab->updBut = newUpdateHistory;
    return returnTab;
}

void MainWindow::on_pushButtonAddChat_clicked()
{
    ui->pushButtonAddChat->setDisabled(true);
    if (m_loginsForSingleChat.isEmpty())
    {
        return;
    }
    saveUniqueChat *newTab = createNewTab(false);
    QObject::connect(newTab->sendBut, &QPushButton::clicked, [newTab, this]()
    {
        QString dateTime = QDate::currentDate().toString(Qt::SystemLocaleShortDate) + " " + QTime::currentTime().toString(Qt::TextDate);
        newTab->charHistory->setTextColor(Qt::darkGreen);
        newTab->charHistory->append(dateTime + " " + m_myLogin + " : "+ newTab->userText->text());
        m_tcpTransfer->sendMessageToServer(m_myLogin, newTab->chatMembers, newTab->userText->text());
        newTab->userText->clear();
    });

    QObject::connect(newTab->userList, &QComboBox::currentTextChanged,
                     [newTab, this]( const QString &newValue)
        {
            newTab->chatMembers.append(newValue);
            ui->tabWidget->setTabText(newTab->tabIndex, "Чат с пользователем " + newValue);
            m_loginsForSingleChat.removeAll(newValue);
            newTab->userList->setDisabled(true);
            if (!m_loginsForSingleChat.isEmpty())
            {
                ui->pushButtonAddChat->setDisabled(false);
            }

            memPlaceForText saveThis;
            saveThis.curTexEdit = newTab->charHistory;
            saveThis.statusLabel = newTab->statusLabel;
            saveThis.receivers.append(newValue);
            saveThis.sender = m_myLogin;
            m_saveTabMap.append(saveThis);

            newTab->charHistory->setEnabled(true);
            newTab->userText->setEnabled(true);
            newTab->sendBut->setEnabled(true);
        }
    );

    QObject::connect(newTab->updBut, &QPushButton::clicked, [newTab, this]()
    {
        QStringList uniqueMembers = m_tabsIndexAndMembersMap.value(newTab->tabIndex);
        m_tcpTransfer->sendTakeHistoryToServer(m_myLogin,  newTab->chatMembers);
    });

    QObject::connect(newTab->userText, &QLineEdit::textEdited, [newTab, this](QString newText)
    {
        m_tcpTransfer->sendStartWriteToServer(m_myLogin, newTab->chatMembers);
    });
}


void MainWindow::on_pushButtonGroupChat_clicked()
{
    saveUniqueChat *newTab = createNewTab(true);

    QStringList uniqueLogins;
    m_tabsIndexAndMembersMap[newTab->tabIndex] = uniqueLogins;

    QObject::connect(newTab->userList, &QComboBox::currentTextChanged,
                     [newTab, this]( const QString &newValue)
        {
            QStringList uniqueMembers = m_tabsIndexAndMembersMap.value(newTab->tabIndex);
            if (!uniqueMembers.contains(newValue))
                uniqueMembers.append(newValue);
            m_tabsIndexAndMembersMap[newTab->tabIndex] = uniqueMembers;

            makeGroupChatTitle(uniqueMembers, newTab->tabIndex);
            checkGroupChatMembers(uniqueMembers, newTab->tabIndex);
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(newTab->userList->model());
            QStandardItem* item = model->item(newTab->userList->findText(newValue));
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);\
            int find_place = -1;
            for (int i = 0; i < m_saveTabMap.size(); i++)
            {
                if (m_saveTabMap[i].curTexEdit == newTab->charHistory)
                {
                    find_place = i;
                    m_saveTabMap[i].receivers.append(newValue);
                    m_saveTabMap[i].receivers.removeDuplicates();
                    break;
                }
            }

            if (find_place == -1)
            {
                memPlaceForText saveThis;
                saveThis.curTexEdit = newTab->charHistory;
                saveThis.statusLabel = newTab->statusLabel;
                saveThis.receivers.append(newValue);
                saveThis.sender = m_myLogin;
                m_saveTabMap.append(saveThis);
            }

            newTab->charHistory->setEnabled(true);
            newTab->userText->setEnabled(true);
            newTab->sendBut->setEnabled(true);

        }
    );

    QObject::connect(newTab->sendBut, &QPushButton::clicked, [newTab, this]()
    {
        QString dateTime = QDate::currentDate().toString(Qt::SystemLocaleShortDate) + " " + QTime::currentTime().toString(Qt::TextDate);
        newTab->charHistory->setTextColor(Qt::darkGreen);
        newTab->charHistory->append(dateTime + " " + m_myLogin + " : "+ newTab->userText->text());
        QStringList uniqueMembers = m_tabsIndexAndMembersMap.value(newTab->tabIndex);
        m_tcpTransfer->sendMessageToServer(m_myLogin, uniqueMembers, newTab->userText->text());
        newTab->userText->clear();
    });

    QObject::connect(newTab->updBut, &QPushButton::clicked, [newTab, this]()
    {
        QStringList uniqueMembers = m_tabsIndexAndMembersMap.value(newTab->tabIndex);
        m_tcpTransfer->sendTakeHistoryToServer(m_myLogin, uniqueMembers);
    });

    QObject::connect(newTab->userText, &QLineEdit::textEdited, [newTab, this](QString newText)
    {
        QStringList uniqueMembers = m_tabsIndexAndMembersMap.value(newTab->tabIndex);
        m_tcpTransfer->sendStartWriteToServer(m_myLogin, uniqueMembers);
    });
}

void MainWindow::checkGroupChatMembers(const QStringList& members, const int& tabIndex)
{
    QMapIterator<int, QStringList> i(m_tabsIndexAndMembersMap);
    QStringList saveMemagers = members;
    while (i.hasNext())
    {
        i.next();
        if (i.key() != tabIndex)
        {
            QStringList forCompare = i.value();
            qSort(saveMemagers);
            qSort(forCompare);
            if (forCompare == saveMemagers)
            {
                ui->statusbar->showMessage("Чат с данными пользователями уже существует. "
                                           "Необходимо добавить ещё пользователей, "
                                           "иначе через 10 секунд произойдёт "
                                           " удаление вкладки.");
                QTimer::singleShot(10000, [this, tabIndex, i]() {
                    QStringList forCompare1 = i.value();
                    QStringList forCompare2 = m_tabsIndexAndMembersMap.value(tabIndex);
                    qSort(forCompare1);
                    qSort(forCompare2);
                    if (forCompare1 == forCompare2)
                    {
                        ui->tabWidget->removeTab(tabIndex);
                        m_tabsIndexAndMembersMap.remove(tabIndex);
                    }
                    ui->statusbar->clearMessage();
                } );
            }
        }
    }
}

void MainWindow::makeGroupChatTitle(const QStringList& members, const int& tabIndex)
{
    QString newTitle;
    for (int i = 0; i < members.size();i++)
    {
        newTitle += members.at(i);
        if (i + 1 < members.size())
            newTitle += "; ";
        else
        {
            newTitle += "; " + m_myLogin;
            newTitle += ".";
        }
    }

    ui->tabWidget->setTabText(tabIndex, "Чат пользователей " + newTitle);
}

void MainWindow::on_pushButAddNew_clicked()
{
    bool ok;
    QString newLogin = QInputDialog::getText(this, "Создание пользователя",
                                         "Новый логин:", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !newLogin.isEmpty())
    {
        bool ok2;
        QString newPassword = QInputDialog::getText(this, "Создание пользователя " + newLogin,
                                             "Пароль для " + newLogin + ":", QLineEdit::Normal,
                                             "", &ok2);
        if (ok2 && !newPassword.isEmpty())
        {
            m_tryCreateLogin = true;
            m_tcpTransfer->sendNewLoginToServer(newLogin, newPassword);
        }

    }
}

