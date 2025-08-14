//#include "dbworker.h"
//
//dbworker::dbworker(MainWindow *mainwindow, Dbworker *dbworker, QObject *parent) : QObject(parent),
//    mainwindow(mainwindow),
//    dbworker(dbworker) {
//
//    dbworkerThread = new QThread(this);
//
//}
    //jlinkThread = new QThread(this);
    //jlinkWorker = new JLinkWorker();
    //jlinkWorker->moveToThread(jlinkThread);
    //connect(jlinkThread, &QThread::finished, jlinkWorker, &QObject::deleteLater);
    //jlinkThread->start();
    //
    //connect(jlinkWorker, &JLinkWorker::outputReceived, this, &FirmwareManager::handleJLinkOutput);
    //connect(jlinkWorker, &JLinkWorker::jlinkStarted, this, &FirmwareManager::onJLinkStarted);
    //connect(jlinkWorker, &JLinkWorker::jlinkFailedToStart, this, &FirmwareManager::onJLinkFailed);
    //connect(jlinkWorker, &JLinkWorker::jlinkFinished, this, &FirmwareManager::onJLinkFinished);
    //
    // Установка таймера для вызова на таймаут
    //timeoutTimer->setSingleShot(true);
    //connect(timeoutTimer, &QTimer::timeout, this, &FirmwareManager::onTimeout);
//}
//FirmwareManager::~FirmwareManager() {
//    if (jlinkThread && jlinkThread->isRunning()) {
//        QMetaObject::invokeMethod(jlinkWorker, "stop", Qt::QueuedConnection);
//        jlinkThread->quit();
//        jlinkThread->wait();
//    }
//}
