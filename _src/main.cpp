#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("PGSS");
    a.setOrganizationName("Propulse NTNU");
    a.setOrganizationDomain("www.propulsentnu.no");
    a.setStyleSheet(":/propulse_styling.qss");

    // Add styling
    QFile styleSheetFile(":/propulse_styling.qss");
    if (styleSheetFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream styleStream(&styleSheetFile);
        a.setStyleSheet(styleStream.readAll());
    } else {
        qDebug() << "Could not open stylesheet.";
    }

    MainWindow w;
    w.show();

    return a.exec();
}
