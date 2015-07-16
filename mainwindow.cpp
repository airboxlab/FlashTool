#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QResource>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Instanciating the threads
    t1=new DFUThread();
    t2=new PortThread();
    //Starting the PortThread
    t2->start();
    this->centralWidget()->setLayout(ui->verticalLayout);

    //Get the firmware full path
    QDir hexFiles = QDir("./Firmware/","*.hex", QDir::Name,QDir::Files);
    QFileInfoList file=hexFiles.entryInfoList();
    filePath=file.at(0).absoluteFilePath();
    //Get the dfu_programmer full path
    hexFiles = QDir("./DFU Programmer/","*.exe",QDir::Name,QDir::Files);
    file = hexFiles.entryInfoList();
    dfuPath = file.at(0).absoluteFilePath();
    //filePath=filePath.replace(" ","\\ ");
    qDebug() << filePath;
    qDebug() << dfuPath;
    ui->display->setVisible(false);
    this->resize(260,100);
    flashing=false;

    //To avoid clicking the button before the first iteration in the PortThread
    lockUi(true);

    //Prepare the ui for the chosen mode
    debuggerMode=DEBUGGER_MODE;
    if (debuggerMode)
    {
        ui->display->setVisible(debuggerMode);
        this->resize(260,300);
    }
    //All the connection
    connect(ui->update,SIGNAL(clicked()),this,SLOT(on_clicked()));
    connect(this,SIGNAL(FlashFirmware(QString,QString,QString)),t1,SLOT(FlashFirmware(QString,QString,QString)));
    connect(t1,SIGNAL(response(QString)),this,SLOT(updateOutput(QString)));
    connect(t2,SIGNAL(isPlugged(bool)),this,SLOT(connectedAirbox(bool)));
    connect(t2,SIGNAL(updateName(QString)),this,SLOT(updatePortName(QString)));
    connect(t1,SIGNAL(updatePB(int)),this,SLOT(updateProgressBar(int)));
    connect(t1,SIGNAL(debugResponse(QString)),this,SLOT(updateDebug(QString)));
    connect(t1,SIGNAL(error(QString)),this,SLOT(displayError(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Display error from the DFU Thread
void MainWindow::displayError(QString errorMsg)
{
    int ret=QMessageBox::information(this, "Result",errorMsg);
    //If we are not in debug mode, clicking ok in the final popup will close app
    if( ret == QMessageBox::Ok && !debuggerMode) qApp->quit();

}

//Update the display with debug response from dfu-programmer
void MainWindow::updateDebug(QString output)
{
    ui->display->append(output);
}

//Update the status bar and the display with messages about the current state of the operation
void MainWindow::updateOutput(QString output)
{
    qDebug() << "update : "+output;
    ui->display->append("<html><b>"+output+"</html></b>");
    ui->statusBar->showMessage(output);
    //If everything worked
    if (output=="Flash successful")
    {
        qDebug()<< "ok";
        //Flash finished
        flashing=false;
        int ret=QMessageBox::information(this, "Result","Flashed successfully");
        //If we are not in debug mode, clicking ok in the final popup will close app
        if( ret == QMessageBox::Ok && !debuggerMode) qApp->quit();
    }
}

//Start the flashing process
void MainWindow::on_clicked()
{
    ui->display->clear();
    if (!t1->isRunning())
    {
        t1->start();
    }
    lockUi(true);
    t2->quit();
    flashing=true;
    qDebug() << "before sending "+filePath;
    emit FlashFirmware(filePath,portName,dfuPath);
}

//if not in flashing mode, lock the ui
//b=true-> airboxlab connected
void MainWindow::connectedAirbox(bool b)
{
    if (!flashing)
    {
        lockUi(!b);
        if (b)
        {
            ui->statusBar->showMessage("Airboxlab detected");
            //enabled=false;
        }
        else
        {
            ui->statusBar->showMessage("Airboxlab not detected : Please plug it");

        }
    }
}

//Update portName
void MainWindow::updatePortName(QString q)
{
    portName=q;
}

//Lock the ui
void MainWindow::lockUi(bool locked)
{
    ui->update->setDisabled(locked);
}


//Update the progress bar
void MainWindow::updateProgressBar(int p)
{
    ui->progressBar->setValue(p);
}
