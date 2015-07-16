#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir dir(QCoreApplication::applicationDirPath());
    //To have the same directory while on Qt, and after deployement
    #if defined(Q_OS_MAC)
    if (dir.dirName() == "MacOS")
    {
        dir.cdUp();
        dir.cdUp();
        //dir.cdUp();
    }
    #endif
    QDir::setCurrent(dir.absolutePath());
    MainWindow w;
    w.show();

    return a.exec();
}
