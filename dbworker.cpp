#include "dbworker.h"
#include "mainwindow.h"

#include <QThread>

dbworker::dbworker(MainWindow *mainwindow, QObject *parent)
    : QObject(parent),
    m_mainwindow(mainwindow),
    dbworkerThread(new QThread) // без родителя
{
    // Если хотите, чтобы слоты данного объекта выполнялись в отдельном потоке,
    // раскомментируйте moveToThread и соответствующие подключения:
    //
    // this->moveToThread(dbworkerThread);
    // connect(dbworkerThread, &QThread::finished, this, &QObject::deleteLater);
    //
    // Запускать поток можно из конструктора или из отдельного метода:
    // dbworkerThread->start();
}

dbworker::~dbworker()
{
    stopThread();
    if (dbworkerThread) {
        delete dbworkerThread;
        dbworkerThread = nullptr;
    }
}

void dbworker::startThread()
{
    if (dbworkerThread && !dbworkerThread->isRunning()) {
        dbworkerThread->start();
    }
}

void dbworker::stopThread()
{
    if (dbworkerThread && dbworkerThread->isRunning()) {
        dbworkerThread->quit();
        dbworkerThread->wait();
    }
}
