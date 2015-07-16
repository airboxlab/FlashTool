#include "portthread.h"
#include "QMessageBox"
#include <QDebug>

PortThread::PortThread(QObject *parent) :
    QThread(parent)
{
    //Contains all the ports available
    serialPortInfoList=QSerialPortInfo::availablePorts();
    ListPort=new QStringList();
    length=serialPortInfoList.length();
    setParent(0);
    moveToThread(this);

}

void PortThread::run()
{
    //b=true if connected
    bool b;
    QString name;
    int i=0;
    //Infinite loop
    while (true)
    {

        b=false;
        //We don't need to update the information every ms
        QThread::msleep(1000);
        //Update the serialPortInfoList
        serialPortInfoList=QSerialPortInfo::availablePorts();
        //If there is more or less port, we update
        if (length!=serialPortInfoList.length() || i==0)
        {
            foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList)
            {
                qDebug() << serialPortInfo.manufacturer();
                //Check the manufacturer to match one the following
                if (serialPortInfo.manufacturer()=="www.airboxlab.com" || serialPortInfo.manufacturer()=="getalima.com" || serialPortInfo.manufacturer()=="http://getalima.com/" || serialPortInfo.manufacturer()=="http://www.airboxlab.com/" || serialPortInfo.manufacturer()=="Dean Camera")
                {
                    b=true;
                    name=*(new QString(serialPortInfo.portName()));
                }
            }

            //Inform MainWindow that the airboxlab is connected
            emit isPlugged(b);
            //Transmit it port name
            emit updateName(name);
        }
        length=serialPortInfoList.length();
        ListPort->clear();
        //}
        i=1;
    }
    exec();
}



