#ifndef DBWORKER_H
#define DBWORKER_H

#include <QObject>
#include <QString>

class MainWindow;
class QThread;

class DbWorker : public QObject
{
    Q_OBJECT
public:
    explicit DbWorker(MainWindow *mainwindow, QObject *parent = nullptr);
    ~DbWorker();

public slots:
    void setBdName(const QString &name);
    void setUserName(const QString &user);
    void setHost(const QString &host);
    void setPort(int port);

    void startThread();
    void stopThread();

    void insertElement(int id, const QString &name, int salary);
    void iterateElement();
    void followInstruct();
    void connectToDb();

signals:
    void errorOccured(const QString &msg);

public:
    QString bdName() const;
    QString userName() const;
    QString host() const;
    int port() const;

private:
    MainWindow *m_mainwindow;
    QThread *dbworkerThread;

    QString m_bdName;
    QString m_userName;
    QString m_host;
    int m_port = 0;

    QString m_password;
};

#endif // DBWORKER_H
