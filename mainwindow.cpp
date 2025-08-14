#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

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

}

MainWindow::~MainWindow()
{
    delete ui;
}
