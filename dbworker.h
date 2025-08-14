#ifndef DBWORKER_H
#define DBWORKER_H

#include <QObject>

class MainWindow;
class QThread;

class dbworker : public QObject
{
    Q_OBJECT
public:
    explicit dbworker(MainWindow *mainwindow, QObject *parent = nullptr);
    ~dbworker();

    void startThread();
    void stopThread();

private:
    MainWindow *m_mainwindow;
    QThread *dbworkerThread;
};

#endif // DBWORKER_H
