#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define DEBUGGER_MODE true;

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QStringList>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include "dfuthread.h"
#include <QFileDialog>
#include <QStringList>
#include "portthread.h"
#include <QMessageBox>
#include <QPalette>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void lockUi(bool locked);
public slots:
    void updateOutput(QString output);
    void updateDebug(QString output);
    void on_clicked();
    void connectedAirbox(bool b);
    void updatePortName(QString q);
    void updateProgressBar(int p);
    void displayError(QString);
private:
    Ui::MainWindow *ui;
    DFUThread *t1;
    PortThread *t2;
    QString filePath;
    QString dfuPath;
    QString portName;
    bool flashing;
    bool debuggerMode;
signals :
    void FlashFirmware(QString filePath,QString portName, QString dfuPath);
};

#endif // MAINWINDOW_H
