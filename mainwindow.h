#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QLineEdit *bdName;
    QPushButton *startBtn;
    QPushButton *stopBtn;
    QLabel *userNameLabel;
    QLineEdit *userName;
    QLabel *hostNumLabel;
    QLineEdit *hostNum;
    QLabel *portNumLabel;
    QLineEdit *portNum;
    QLabel *tableName;
    QTableView *tableBD;
    QLabel *bdNameLabel;

    QHBoxLayout *mainHLayout;
    QVBoxLayout *mainV1Layout;
    QVBoxLayout *mainV2Layout;
    QVBoxLayout *mainV3Layout;
    QHBoxLayout *mainH1Layout;
    QHBoxLayout *mainH2Layout;
    QHBoxLayout *mainH3Layout;
};
#endif // MAINWINDOW_H
