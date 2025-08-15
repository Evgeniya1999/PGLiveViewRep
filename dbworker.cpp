#include "dbworker.h"
#include "mainwindow.h"

#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>

DbWorker::DbWorker(MainWindow *mainwindow, QObject *parent)
    : QObject(parent),
    m_mainwindow(mainwindow),
    dbworkerThread(new QThread)
{
    this->moveToThread(dbworkerThread);
    connect(dbworkerThread, &QThread::finished, this, &QObject::deleteLater);
}

DbWorker::~DbWorker()
{
    stopThread();
    if (dbworkerThread) {
        delete dbworkerThread;
        dbworkerThread = nullptr;
    }
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

// реализация с параметрами (соответствует объявлению в header)
void DbWorker::insertElement(int id, const QString &name, int salary)
{
    const QString connectionName = QStringLiteral("mydb_connection");

    if (!QSqlDatabase::contains(connectionName)) {
        qWarning() << "insertElement: no such database connection:" << connectionName;
        emit errorOccured("Нет соединения с БД");
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "insertElement: database is not open";
        emit errorOccured("База данных не открыта");
        return;
    }

    // начинаем транзакцию если поддерживается
    if (!db.transaction()) {
        qWarning() << "insertElement: cannot start transaction:" << db.lastError().text();
        // не обязательно прекращать, можно продолжить без транзакции
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO employee (id, name, salary) VALUES (:id, :name, :salary)");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":salary", salary);
    if (!query.exec()) {
        qWarning() << "insertElement exec error:" << query.lastError().text();
        db.rollback();
        emit errorOccured(query.lastError().text());
        return;
    }
    if (!db.commit()) {
        qWarning() << "insertElement: commit failed:" << db.lastError().text();
        emit errorOccured(db.lastError().text());
    } else {
        qDebug() << "insertElement: inserted" << id << name << salary;
    }
}

void DbWorker::iterateElement()
{
    const QString connectionName = QStringLiteral("mydb_connection");
    QSqlQuery query(QSqlDatabase::database(connectionName));
    if (!query.exec("SELECT name, salary FROM employee")) {
        qWarning() << "iterateElement exec error:" << query.lastError().text();
        emit errorOccured(query.lastError().text());
        return;
    }

    while (query.next()) {
        QString name = query.value(0).toString();
        int salary = query.value(1).toInt();
        qDebug() << name << salary;
    }
}

void DbWorker::followInstruct()
{
    const QString connectionName = QStringLiteral("mydb_connection");
    if (!QSqlDatabase::contains(connectionName)) {
        emit errorOccured(tr("Нет соединения с БД"));
        return;
    }

    QSqlQuery query(QSqlDatabase::database(connectionName));
    if (!query.exec("SELECT name, salary FROM employee WHERE salary > 50000")) {
        qWarning() << "followInstruct error:" << query.lastError().text();
        emit errorOccured(query.lastError().text());
        return;
    }

    while (query.next()) {
        const QString name = query.value(0).toString();
        const int salary = query.value(1).toInt();
        qDebug() << "followInstruct:" << name << salary;
    }
}

void DbWorker::connectToDb()
{
    const QString connName = QStringLiteral("mydb_connection");

    // Если такое соединение уже есть — удалим его
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase old = QSqlDatabase::database(connName);
        if (old.isOpen())
            old.close();
        QSqlDatabase::removeDatabase(connName);
    }

    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        qWarning() << "QPSQL driver not available";
        emit errorOccured(QStringLiteral("QPSQL driver not available"));
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), connName);
    db.setHostName(m_host.isEmpty() ? QStringLiteral("bigblue") : m_host);
    db.setPort(m_port > 0 ? m_port : 5432);
    db.setDatabaseName(m_bdName.isEmpty() ? QStringLiteral("work_db") : m_bdName);
    db.setUserName(m_userName.isEmpty() ? QStringLiteral("Evgeniya") : m_userName);
    //db.setPassword(m_password);
    db.setPassword("1234");

    if (!db.open()) {
        qWarning() << "DB open error:" << db.lastError().text();
        emit errorOccured(db.lastError().text());
        db.close();
        QSqlDatabase::removeDatabase(connName);
        return;
    }

    qDebug() << "DB connected (connectionName):" << db.connectionName();
}

void DbWorker::startThread()
{
    if (dbworkerThread && !dbworkerThread->isRunning()) {
        dbworkerThread->start();
    }
}

void DbWorker::stopThread()
{
    if (dbworkerThread && dbworkerThread->isRunning()) {
        dbworkerThread->quit();
        dbworkerThread->wait();
    }
}
