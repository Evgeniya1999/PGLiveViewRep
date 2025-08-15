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

    bdNameLabel = new QLabel("Имя БД: ");
    bdName = new QLineEdit(this);
    startBtn = new QPushButton("Старт");
    stopBtn = new QPushButton("Стоп");
    userNameLabel = new QLabel("Пользователь: ");
    userName = new QLineEdit(this);
    hostNumLabel = new QLabel("Хост: ");
    hostNum = new QLineEdit(this);
    portNumLabel = new QLabel("Порт: ");
    portNum = new QLineEdit(this);
    tableName = new QLabel("BD LiveView");
    tableBD = new QTableView(this);

    tableBD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    central->setLayout(rootLayout);
    setCentralWidget(central);

    rootLayout->addWidget(tableName, 0, Qt::AlignHCenter);
    rootLayout->addWidget(tableBD, /*stretch=*/1);

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
    mainV1Layout->addWidget(startBtn);
    mainV2Layout->addWidget(stopBtn);

    dbworker = new DbWorker(this);

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::connectToBD);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::disconnectToBD);
    connect(bdName, &QLineEdit::textChanged, this, &MainWindow::bdNameWrite);
    connect(userName, &QLineEdit::textChanged, this, &MainWindow::userNameWrite);
    connect(hostNum, &QLineEdit::textChanged, this, &MainWindow::hostNumWrite);
    connect(portNum, &QLineEdit::textChanged, this, &MainWindow::portNumWrite);

    connect(dbworker, &DbWorker::errorOccured, this, [this](const QString &err){
        QMessageBox::warning(this, tr("Ошибка подключения к БД"), err);
    });

    tableBD->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableBD->setSelectionMode(QAbstractItemView::SingleSelection);
    tableBD->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
    dbworker->connectToDb();
    const QString connName = QStringLiteral("mydb_connection");
    if (!QSqlDatabase::contains(connName)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Подключение не создано: %1").arg(connName));
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(connName);
    if (!db.isValid() || !db.isOpen()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Соединение с БД не открыто"));
        return;
    }

    if (g_model) {
        delete g_model;
        g_model = nullptr;
        tableBD->setModel(nullptr);
    }

    g_model = new QSqlTableModel(this, db);
    g_model->setTable(QStringLiteral("WorkTableDataBase"));
    g_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    if (!g_model->select()) {
        QString err = g_model->lastError().text();
        delete g_model;
        g_model = nullptr;
        QMessageBox::critical(this, tr("Ошибка запроса"), err);
        return;
    }

    tableBD->setModel(g_model);
    tableBD->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    qDebug() << "Модель подключена к tableView";
}

void MainWindow::disconnectToBD()
{
    const QString connName = QStringLiteral("mydb_connection");

    if (g_model) {
        tableBD->setModel(nullptr);
        delete g_model;
        g_model = nullptr;
    }

    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase db = QSqlDatabase::database(connName);
        if (db.isOpen()) db.close();
        QSqlDatabase::removeDatabase(connName);
    }
    qDebug() << "Отключение от БД выполнено";
}

MainWindow::~MainWindow()
{
    disconnectToBD();
    delete ui;
}
