#ifndef DFUTHREAD_H
#define DFUTHREAD_H

#include <QThread>
#include <QSerialPort>
#include <QProcess>
#include <QDebug>

class DFUThread : public QThread
{
    Q_OBJECT
public:
    explicit DFUThread(QObject *parent = 0);
    QSerialPort *serial;
    //Process to run the dfu-programmer command
    QProcess *sh;
    //Get the terminal message for the i step
    QString GetTermMessage(QString,int);
signals:
    //emit a signal with a small message informing the user of the operation status
    void response(QString termResponse);
    //emit a signal with the full message from dfu-programmer debug mode
    void debugResponse(QString debugResponse);
    //emit a signal to update the progress bar current value
    void updatePB(int p);
    void error(QString);
protected:
    void run();
public slots:
    //Run the full flash routine taking in parameters the *.hex file path and the serial port name
    void FlashFirmware(QString filePath, QString portName, QString dfuPath);
};

#endif // DFUTHREAD_H
