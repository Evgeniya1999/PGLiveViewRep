#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QLineEdit;
class QPushButton;
class QLabel;
class QTableView;
class QVBoxLayout;
class QHBoxLayout;
class DbWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateModel();

    void bdNameWrite(const QString &text);
    void userNameWrite(const QString &text);
    void hostNumWrite(const QString &text);
    void portNumWrite(const QString &text);
    void tableNameWrite(const QString &text);
    void passwordWrite(const QString &text);
    void connectToBD();
    void disconnectToBD();
private:
    Ui::MainWindow *ui;
    QLineEdit *bdName;
    QLabel *bdNameLabel;
    QLineEdit *userName;
    QLabel *userNameLabel;
    QLineEdit *hostNum;
    QLabel *hostNumLabel;
    QLineEdit *portNum;
    QLabel *portNumLabel;
    QLineEdit *password;
    QLabel *passwordLabel;

    QLineEdit *tableName;
    QLabel *tableNameLabel;
    QTableView *tableBD;

    QPushButton *startBtn;
    QPushButton *stopBtn;

    QHBoxLayout *mainHLayout;
    QVBoxLayout *mainV1Layout;
    QVBoxLayout *mainV2Layout;
    QVBoxLayout *mainV3Layout;
    QHBoxLayout *mainH1Layout;
    QHBoxLayout *mainH2Layout;
    QHBoxLayout *mainH3Layout;

    QString bdNameText;
    QString userNameText;
    QString hostNumText;
    QString portNumText;
    QString tableNameText;
    QString passwordText;

    DbWorker *dbworker;
};

#endif // MAINWINDOW_H
