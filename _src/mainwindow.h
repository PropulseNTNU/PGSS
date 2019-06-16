#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "globals.h"
#include <QtCharts/QChartView>
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
     void resetVisuals();

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
    QLabel* drogueStateLbl;
    QLabel* chuteStateLbl;
    QLabel* landedStateLbl;
    LightWidget* armedStateLight;
    LightWidget* burnoutStateLight;
    LightWidget* airbrakesStateLight;
    LightWidget* apogeeStateLight;
    LightWidget* drogueStateLight;
    LightWidget* chuteStateLight;
    LightWidget* landedStateLight;
    void updateStateVisual(globals::state state);
    globals::state currentState;

    void updateMissionTime();
    QTimer* missionTimer;
    unsigned int hours;
    unsigned int min;
    unsigned int secs;
    unsigned int msecs;

    void createDataSection();
    QWidget* dataSectionContainer;

    QWidget* topDataContainer;
    QLabel* topDataLabel;
    QLabel* altitudeLeftLbL;
    QLabel* altitudeRightLbl;
    QLabel* maxAltitudeLeftLbL;
    QLabel* maxAltiudeRightLbl;
    QLabel* velocityLeftLbL;
    QLabel* velocityRightLbl;
    QLabel* maxVelocityLeftLbL;
    QLabel* maxVelocityRightLbl;
    QLabel* accelerationLeftLbL;
    QLabel* accelerationRightLbl;
    QLabel* gpsLeftLbl;
    QLabel* gpsMidLbl;
    QLabel* gpsRightLbl;

    QWidget* bottomDataContainer;
    QLabel* bottomDataLabel;
    QLabel* pitchLeftLbl;
    QLabel* pitchRightLbl;
    QLabel* rollLeftLbl;
    QLabel* rollRightLbl;
    QLabel* yawLeftLbl;
    QLabel* yawRightLbl;
    QLabel* pitchRateLeftLbl;
    QLabel* pitchRateRightLbl;
    QLabel* rollRateLeftLbl;
    QLabel* rollRateRightLbl;
    QLabel* yawRateLeftLbl;
    QLabel* yawRateRightLbl;

    void createChartViews();
    QWidget* chartContainer;
    RealTimeChart* altitudeChart;
    RealTimeChart* accelerationChart;
    QtCharts::QChartView* altitudeChartView;
    QtCharts::QChartView* accelerationChartView;
    QTimer* chartTimer;

    void createMenuBar();
    QMenuBar* menuBar;
    QMenu* addMenu;
    QMenu* deviceMenu;
    QMenu* settingsMenu;
    QMenu* viewMenu;

    void createNavball();
    NavballWidget* navballWidget;

    void createGPSMap();
    QQuickWidget* gpsMapView;
    QWidget* gpsMapWidget;

    void createControlWidget();
    ControlWidget* controlWidget;

    SerialInterface* serialInterface;
    QString currentPort;
    bool visualsPaused;
    double maxAltitude;
    double prevPitch;
    double prevRoll;
    double prevYaw;
};

#endif // MAINWINDOW_H
