#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QMainWindow>
#include <QVector>
#include <QFile>

class ControlWidget;
class RealTimeChart;
class SerialInterface;
class NavballWidget;
class LightWidget;
class QTimer;
class QListWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QGroupBox;
class QMenuBar;
class QMenu;
class QAction;
class QPixmap;
class QLabel;
class QQuickWidget;
class QListWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
     void showAvailablePorts();
     void updateRealTimeVisuals();

private:
    void createCentralWidget();
    QWidget* centralWidget;

    void createStatusBar();
    QWidget* statusBarContainer;
    QLabel* logoLbl;
    QPixmap* logoPixmap;
    QWidget* statusMidContainer;
    QLabel* missionTimeLbl;
    QLabel* timeLbl;
    QLabel* timeExpLbl;
    QWidget* statusRightContainer;
    QLabel* armedStateLbl;
    QLabel* burnoutStateLbl;
    QLabel* airbrakesStateLbl;
    QLabel* apogeeStateLbl;
    QLabel* landingStateLbl;
    LightWidget* armedStateLight;
    LightWidget* burnoutStateLight;
    LightWidget* airbrakesStateLight;
    LightWidget* apogeeStateLight;
    LightWidget* landingStateLight;

    void createChartViews();
    RealTimeChart* altitudeChart;
    RealTimeChart* accelerationChart;
    QtCharts::QChartView* altitudeChartView;
    QtCharts::QChartView* accelerationChartView;

    void createMenuBar();
    QMenuBar* menuBar;
    QMenu* addMenu;
    QMenu* deviceMenu;

    void createNavball();
    NavballWidget* navballWidget;

    void createGPSMap();
    QQuickWidget* gpsMapView;
    QWidget* gpsMapWidget;

    ControlWidget* controlWidget;

    SerialInterface* serialInterface;
    QString currentPort;
    QTimer* timer;
    ControlWidget* test;
};

#endif // MAINWINDOW_H
