#ifndef DBWORKER_H
#define DBWORKER_H

#include <QObject>
#include <QThread>
#include <QVariant>
#include <QtSql/QSqlDriver>

class MainWindow;

class DbWorker : public QObject
{
    Q_OBJECT
public:
    explicit DbWorker(MainWindow *mainwindow, QObject *parent = nullptr);
    ~DbWorker();

    void setTableName(const QString &tbname);
    void setPassword(const QString &pass);
    void setBdName(const QString &name);
    void setUserName(const QString &user);
    void setHost(const QString &host);
    void setPort(int port);

    QString bdName() const;
    QString userName() const;
    QString host() const;
    int port() const;
    QString tableName() const;
    QString pass() const;

    void startThread();

public slots:
    void connectToDb();
    void disconnect();
    void stopThread();
private slots:
    void onDriverNotification(const QString &name, QSqlDriver::NotificationSource src, const QVariant &payload);

signals:
    void errorOccured(const QString &err);
    //сигнал, который будет отправлен при получении уведомления от БД
    void notificationReceived(const QString &name, const QVariant &payload);

private:
    MainWindow *m_mainwindow;
    QThread *m_thread;
    QString m_connectionName;
    QString m_tbName;
    QString m_password;
    QString m_bdName;
    QString m_userName;
    QString m_host;
    int m_port;
};

#endif // DBWORKER_H
