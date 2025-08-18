#include "dbworker.h"
#include "mainwindow.h"

#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlDriver>
#include <QDebug>
#include <QUuid>
#include <QMetaObject>

DbWorker::DbWorker(MainWindow *mainwindow, QObject *parent)
    : QObject(parent),
    m_mainwindow(mainwindow),
    m_thread(new QThread)
{
    this->moveToThread(m_thread);
    connect(this, SIGNAL(stopThread()), this, SLOT(disconnect()), Qt::BlockingQueuedConnection);
    connect(m_thread, &QThread::finished, this, &QObject::deleteLater);
}

DbWorker::~DbWorker()
{
    stopThread();
    if (m_thread) {
        if (m_thread->isRunning()) {
            m_thread->quit();
            m_thread->wait();
        }
    }
}

void DbWorker::setTableName(const QString &tbname)
{
    m_tbName = tbname;
    qDebug() << "DbWorker::setTableName =" << m_tbName;
}
void DbWorker::setPassword(const QString &pass)
{
    m_password = pass;
    qDebug() << "DbWorker::setPassword =" << m_password;
}
void DbWorker::setBdName(const QString &name)
{
    m_bdName = name;
    qDebug() << "DbWorker::setBdName =" << m_bdName;
}

void DbWorker::setUserName(const QString &user)
{
    m_userName = user;
    qDebug() << "DbWorker::setUserName =" << m_userName;
}

void DbWorker::setHost(const QString &host)
{
    m_host = host;
    qDebug() << "DbWorker::setHost =" << m_host;
}

void DbWorker::setPort(int port)
{
    m_port = port;
    qDebug() << "DbWorker::setPort =" << m_port;
}

QString DbWorker::bdName() const { return m_bdName; }
QString DbWorker::userName() const { return m_userName; }
QString DbWorker::host() const { return m_host; }
int DbWorker::port() const { return m_port; }
QString DbWorker::tableName() const {return m_tbName;}
QString DbWorker::pass() const {return m_password;}

void DbWorker::onDriverNotification(const QString &name,
                                    QSqlDriver::NotificationSource /*src*/,
                                    const QVariant &payload)
{
    qDebug() << "Получено уведомление от БД" << name << "Тип:" << payload.toString();
    emit notificationReceived(name, payload);
}
void DbWorker::connectToDb()
{
    //создание подключения к бд с уникальным именем
    m_connectionName = QStringLiteral("worker_conn_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        emit errorOccured("QPSQL driver not available");
        m_connectionName.clear();
        return;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), m_connectionName);
    db.setHostName(m_host.isEmpty() ? QStringLiteral("localhost") : m_host);
    db.setPort(m_port);
    db.setDatabaseName(m_bdName.isEmpty() ? QStringLiteral("work_db") : m_bdName);
    db.setUserName(m_userName.isEmpty() ? QStringLiteral("postgres") : m_userName);
    db.setPassword(m_password);

    if (!db.open()) {
        emit errorOccured(db.lastError().text());
        QSqlDatabase::removeDatabase(m_connectionName);
        m_connectionName.clear();
        return;
    }
    //подписка на канал (название совпадает с тем, что отправляет NOTIFY)
    const QString channel = QStringLiteral("insert_users_notf");
    if (!db.driver()->subscribeToNotification(channel)) {
        qWarning() << "подписка не удалась" << db.driver()->lastError().databaseText();
    }
    if (db.driver()) {
        connect(db.driver(),
                SIGNAL(notification(const QString&, QSqlDriver::NotificationSource, const QVariant&)),
                this,
                SLOT(onDriverNotification(const QString&, QSqlDriver::NotificationSource, const QVariant&)));
    } else {
        qWarning() << "Драйвер БД отсутствует!";
    }
    qDebug() << "Подключение рабочего потока к БД выполнено как: " << m_connectionName;
}
void DbWorker::disconnect()
{
    if (m_connectionName.isEmpty())
        return;
    if (QSqlDatabase::contains(m_connectionName)) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            if (db.isOpen())
                db.close();
        }
        QSqlDatabase::removeDatabase(m_connectionName);
        qDebug() << "Удалено подключение:" << m_connectionName;
    }
    m_connectionName.clear();
}

void DbWorker::startThread()
{
    if (m_thread && !m_thread->isRunning()) {
        m_thread->start();
    }
}

void DbWorker::stopThread()
{
    if (m_thread && m_thread->isRunning()) {
        QMetaObject::invokeMethod(this, "closeConnection", Qt::BlockingQueuedConnection);
        m_thread->quit();
        m_thread->wait();
    }
}
