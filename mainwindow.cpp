#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dbworker.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QMetaObject>

static QSqlTableModel *g_model = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(700, 500);
    QWidget *central = new QWidget(this);
    QVBoxLayout *rootLayout = new QVBoxLayout(central);
    mainHLayout = new QHBoxLayout();
    rootLayout->addLayout(mainHLayout);

    mainV1Layout = new QVBoxLayout(this);
    mainV2Layout = new QVBoxLayout(this);
    mainV3Layout = new QVBoxLayout(this);

    mainH1Layout = new QHBoxLayout(this);
    mainH2Layout = new QHBoxLayout(this);
    mainH3Layout = new QHBoxLayout(this);

    bdNameLabel = new QLabel("Имя БД:");
    bdName = new QLineEdit(this);
    userNameLabel = new QLabel("Пользователь:");
    userName = new QLineEdit(this);
    hostNumLabel = new QLabel("Хост:");
    hostNum = new QLineEdit(this);
    portNumLabel = new QLabel("Порт:");
    portNum = new QLineEdit(this);
    tableNameLabel = new QLabel("Имя таблицы:");
    tableName = new QLineEdit(this);
    passwordLabel = new QLabel("Пароль:");
    password = new QLineEdit(this);
    tableBD = new QTableView(this);

    startBtn = new QPushButton("Старт");
    stopBtn = new QPushButton("Стоп");

    tableBD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    central->setLayout(rootLayout);
    setCentralWidget(central);

    rootLayout->addWidget(tableName, 0, Qt::AlignHCenter);
    rootLayout->addWidget(tableBD, 1);

    mainHLayout->addLayout(mainV1Layout);
    mainHLayout->addLayout(mainV2Layout);
    mainHLayout->addLayout(mainV3Layout);

    mainV1Layout->addWidget(bdNameLabel);
    mainV1Layout->addWidget(bdName);
    mainV2Layout->addWidget(userNameLabel);
    mainV2Layout->addWidget(userName);
    mainV1Layout->addWidget(hostNumLabel);
    mainV1Layout->addWidget(hostNum);
    mainV2Layout->addWidget(portNumLabel);
    mainV2Layout->addWidget(portNum);
    mainV1Layout->addWidget(tableNameLabel);
    mainV1Layout->addWidget(tableName);
    mainV2Layout->addWidget(passwordLabel);
    mainV2Layout->addWidget(password);
    mainV1Layout->addWidget(startBtn);
    mainV2Layout->addWidget(stopBtn);

    dbworker = new DbWorker(this);

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::connectToBD);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::disconnectToBD);
    connect(bdName, &QLineEdit::textChanged, this, &MainWindow::bdNameWrite);
    connect(userName, &QLineEdit::textChanged, this, &MainWindow::userNameWrite);
    connect(hostNum, &QLineEdit::textChanged, this, &MainWindow::hostNumWrite);
    connect(portNum, &QLineEdit::textChanged, this, &MainWindow::portNumWrite);
    connect(tableName, &QLineEdit::textChanged, this, &MainWindow::tableNameWrite);
    connect(password, &QLineEdit::textChanged, this, &MainWindow::passwordWrite);

    connect(dbworker, &DbWorker::notificationReceived, this, &MainWindow::updateModel);

    connect(dbworker, &DbWorker::errorOccured, this, [this](const QString &err){
        QMessageBox::warning(this, tr("Ошибка подключения к БД"), err);
    });

    tableBD->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableBD->setSelectionMode(QAbstractItemView::SingleSelection);
    tableBD->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
void MainWindow::updateModel()
{
    if (g_model && g_model->database().isOpen()) {
        qDebug() << "Обновление модели...";
        g_model->select();
    }
}
void MainWindow::tableNameWrite(const QString &text){
    tableNameText = text;
    dbworker->setTableName(text);
}
void MainWindow::passwordWrite(const QString &text){
    passwordText = text;
    dbworker->setPassword(text);
}
void MainWindow::bdNameWrite(const QString &text){
    bdNameText = text;
    dbworker->setBdName(text);
}

void MainWindow::userNameWrite(const QString &text){
    userNameText = text;
    dbworker->setUserName(text);
}

void MainWindow::hostNumWrite(const QString &text){
    hostNumText = text;
    dbworker->setHost(text);
}

void MainWindow::portNumWrite(const QString &text){
    portNumText = text;
    bool ok = false;
    int p = text.toInt(&ok);
    if (ok) {
        dbworker->setPort(p);
    } else {
        dbworker->setPort(0);
    }
}

void MainWindow::connectToBD()
{
    const QString uiConnName = QStringLiteral("ui_connection");
    if (QSqlDatabase::contains(uiConnName)) {
        QSqlDatabase dbOld = QSqlDatabase::database(uiConnName);
        if (dbOld.isOpen()) dbOld.close();
        QSqlDatabase::removeDatabase(uiConnName);
    }

    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        QMessageBox::critical(this, tr("Ошибка"), tr("QPSQL драйвер не доступен"));
        return;
    }

    QSqlDatabase uiDb = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), uiConnName);
    uiDb.setHostName(hostNumText.isEmpty() ? QStringLiteral("0.0.0.0") : hostNumText);
    bool ok = false;
    int port = portNumText.toInt(&ok);
    uiDb.setPort(ok && port > 0 ? port : 5432);
    uiDb.setDatabaseName(bdNameText.isEmpty() ? QStringLiteral("default") : bdNameText);
    uiDb.setUserName(userNameText.isEmpty() ? QStringLiteral("default") : userNameText);
    uiDb.setPassword(passwordText);

    if (!uiDb.open()) {
        QString err = uiDb.lastError().text();
        QSqlDatabase::removeDatabase(uiConnName);
        QMessageBox::critical(this, tr("Ошибка подключения"), err);
        return;
    }

    if (g_model) {
        delete g_model;
        g_model = nullptr;
        tableBD->setModel(nullptr);
    }

    g_model = new QSqlTableModel(this, uiDb);
    const QString tbl = tableNameText.isEmpty() ? QStringLiteral("default") : tableNameText;
    g_model->setTable(tbl);
    g_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    if (!g_model->select()) {
        QString err = g_model->lastError().text();
        delete g_model;
        g_model = nullptr;
        QSqlDatabase db = QSqlDatabase::database(uiConnName);
        if (db.isOpen()) db.close();
        QSqlDatabase::removeDatabase(uiConnName);
        QMessageBox::critical(this, tr("Ошибка запроса"), err);
        return;
    }

    tableBD->setModel(g_model);
    tableBD->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    dbworker->startThread();
    QMetaObject::invokeMethod(dbworker, "connectToDb", Qt::QueuedConnection);
}
void MainWindow::disconnectToBD()
{
    const QString uiConnName = QStringLiteral("ui_connection");
    if (g_model) {
        tableBD->setModel(nullptr);
        delete g_model;
        g_model = nullptr;
    }

    if (QSqlDatabase::contains(uiConnName)) {
        QSqlDatabase db = QSqlDatabase::database(uiConnName);
        if (db.isOpen()) db.close();
        QSqlDatabase::removeDatabase(uiConnName);
    }
    qDebug() << "Соединение закрыто";

    if (dbworker) {
        dbworker->stopThread();
    }
    qDebug() << "Остановка работы";
}

MainWindow::~MainWindow()
{
    disconnectToBD();
    delete ui;
}
