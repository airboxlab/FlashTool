#include "dfuthread.h"

DFUThread::DFUThread(QObject *parent) :
    QThread(parent)
{
    serial=new QSerialPort();
    sh=new QProcess();
    //To be able to use slot in this thread, not in GUI thread.
    setParent(0);
    moveToThread(this);
}

void DFUThread::run()
{
    exec();
}

//Put the airboxlab in bootloader mode, erase, flash with the firmware embedded in the firmware folder in the app bundle, then reset.
void DFUThread::FlashFirmware(QString filePath,QString portName,QString dfuPath)
{
    qDebug() << "After sending : "+ filePath;
    QString temp;
    emit response("Entering in bootloader mode...");
    serial->setPortName(portName);
    qDebug() << portName;
    //Open and close the serial port with 1200 baudrate in order to switch the airboxlab in bootloader mode
    if (serial->open(QIODevice::ReadWrite)) {
        if (serial->isOpen())
        {
            qDebug() << serial->isOpen();
        if (serial->setBaudRate(QSerialPort::Baud1200,QSerialPort::AllDirections))
        {
            serial->setDataTerminalReady(true);;
            qDebug() << serial->baudRate();
            serial->close();
            qDebug() << serial->isOpen();
            emit updatePB(10);

            //To be sure it's closed
            if (!serial->isOpen())
            {
                //We wait the airboxlab to be in bootloader mode, it take several seconds
                QThread::msleep(5000);
                emit response("In bootloader mode \n");
                emit updatePB(20);
                //Open a process
                sh->setProcessChannelMode(QProcess::MergedChannels);
                emit response("Erasing ...");
                //Launch the dfu-programmer erase command
#if defined (Q_OS_WIN)
                QStringList arguments;
                arguments << "atmega32u4" << "erase" << "--debug" << "2";
                sh->start(dfuPath,arguments);
#elif defined (Q_OS_MAC)
                sh->start("/usr/local/bin/dfu-programmer atmega32u4 erase --debug 2");
#endif
                if(sh->waitForFinished())
                {
                    temp=sh->readAll();
                    //Check the last line of the response, if it's empty, everything went well
                    emit debugResponse(GetTermMessage(temp,1));
                    //If no message, no problem
                    if (!GetTermMessage(temp,1).contains(QRegExp("[A-Za-z]")))
                    {
                        emit response("Erased");
                        emit updatePB(40);
                        sh->close();
                        emit response("Flashing...");
                        QThread::msleep(500);
                        //dfu-programmer flashing command
#if defined (Q_OS_WIN)
                        arguments.clear();
                        arguments << "atmega32u4" << "flash" << filePath << "--debug" << "2";
                        sh->start(dfuPath,arguments);
#elif defined (Q_OS_MAC)
                        sh->start("/usr/local/bin/dfu-programmer atmega32u4 flash \""+filePath+"\" --debug 2");
#endif
                        if (sh->waitForFinished())
                        {
                            temp=sh->readAll();
                            emit debugResponse(GetTermMessage(temp,2));
                            //Check the last line of the response, if it contans "bytes used", everything went well
                            if (GetTermMessage(temp,2).contains("bytes used"))
                            {
                                emit response("Flashed");

                                sh->close();
                                emit updatePB(90);
                                emit response("Back in normal mode ...");
                                QThread::msleep(500);
                                //To finish, we reset the airboxlab with the dfu-programmer reset command
#if defined (Q_OS_WIN)
                                arguments.clear();
                                arguments << "atmega32u4" << "reset";
                                sh->start(dfuPath,arguments);
#elif defined (Q_OS_MAC)
                                sh->start("/usr/local/bin/dfu-programmer atmega32u4 reset");
#endif
                                sh->waitForFinished();
                                if (sh->readAll()=="")
                                {
                                    emit response("Flash successful");
                                    sh->close();
                                    emit updatePB(100);
                                }
                            }
                            else
                            {
                                emit response("Error while flashing");
                                emit error("Flashing error : \n"+GetTermMessage(temp,2));
                            }
                        }
                    }
                    else
                    {
                        emit response ("Error while erasing");
                        emit error("Erasing error : \n"+ GetTermMessage(temp,1));
                        sh->close();
                    }
                }

            }
            else emit error("Not in bootloaded mode");

        } else
        {
            emit error("Can't connect to the serial port");
        }

}
    }
}


//To get the terminal message (last lines) of the i step (1=erasing, 2=flashing,3=reset)
QString DFUThread::GetTermMessage(QString q,int i)
{
    //Choose the separator for each cases
    QString sep;
    switch(i)
    {
    case 1:
        sep="true";
        break;
    case 2:
        sep="hex";
        break;
    case 3:
        sep="";
        break;
    default:
        sep="";
        break;
    }
    QStringList temp=QString(q).split(QRegExp(sep));
    return (temp.at(temp.length()-1));
}
