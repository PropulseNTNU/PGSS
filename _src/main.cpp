#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("PGSS");
    a.setOrganizationName("Propulse NTNU");
    a.setOrganizationDomain("www.propulsentnu.no");
    a.setStyleSheet(":/propulse_styling.qss");

    MainWindow w;
    w.show();

    return a.exec();
}
