#include "mainwindow.h"
#include "globals.h"
#include "realtimechart.h"
#include "serialinterface.h"
#include "xbee.h"
#include "navballwidget.h"
#include "controlwidget.h"
#include "lightwidget.h"

#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QMenuBar>
#include <QAction>
#include <QLineEdit>
#include <QListWidget>
#include <QDockWidget>
#include <QPixmap>
#include <QLabel>
#include <QQuickWidget>
#include <QQuickView>
#include <QVector2D>
#include <QQuickItem>
#include <QTextStream>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), currentPort("")
{
    // Create a serial interface
    serialInterface = new SerialInterface(this);

    // Create widgets
    createChartViews();
    createMenuBar();
    createGPSMap();
    createStatusBar();
    createCentralWidget();

    // Set central widget
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createCentralWidget()
{
    centralWidget = new QWidget;
    controlWidget = new ControlWidget(this);

    QGridLayout* centralLayout = new QGridLayout;
    centralLayout->addWidget(altitudeChartView);
    centralLayout->addWidget(accelerationChartView);
    centralWidget->setLayout(centralLayout);
}

void MainWindow::createStatusBar()
{
    statusBarContainer = new QWidget;

    logoLbl = new QLabel;
    logoPixmap = new QPixmap(":/propulse_logo.png");
    logoLbl->setPixmap(*logoPixmap);
    logoLbl->setScaledContents(true);
    logoLbl->setMaximumSize(QSize(150, 75));

    statusMidContainer = new QWidget;
    missionTimeLbl = new QLabel("Mission Time:");
    timeLbl = new QLabel("00:00:00:00");
    timeExpLbl = new QLabel("HH:MM:SS:MS");
    QVBoxLayout* statusMidLayout = new QVBoxLayout;
    statusMidLayout->addWidget(missionTimeLbl, Qt::AlignLeft);
    statusMidLayout->addWidget(timeLbl, Qt::AlignLeft);
    statusMidLayout->addWidget(timeExpLbl, Qt::AlignLeft);
    statusMidContainer->setLayout(statusMidLayout);

    statusRightContainer = new QWidget;
    armedStateLbl = new QLabel("ARMED");
    burnoutStateLbl = new QLabel("BURNOUT");
    airbrakesStateLbl = new QLabel("BRAKES");
    apogeeStateLbl = new QLabel("APOGEE");
    landingStateLbl = new QLabel("LANDING");
    armedStateLight = new LightWidget(QColor(Qt::green));
    burnoutStateLight = new LightWidget(QColor(Qt::green));
    airbrakesStateLight = new LightWidget(QColor(Qt::green));
    apogeeStateLight = new LightWidget(QColor(Qt::green));
    landingStateLight = new LightWidget(QColor(Qt::green));
    QGridLayout* statusRightLayout = new QGridLayout;
    statusRightLayout->addWidget(armedStateLight, 0, 0, Qt::AlignCenter);
    statusRightLayout->addWidget(burnoutStateLight, 0, 1, Qt::AlignCenter);
    statusRightLayout->addWidget(airbrakesStateLight, 0, 2, Qt::AlignCenter);
    statusRightLayout->addWidget(apogeeStateLight, 0, 3, Qt::AlignCenter);
    statusRightLayout->addWidget(landingStateLight, 0, 4, Qt::AlignCenter);
    statusRightLayout->addWidget(armedStateLbl, 1, 0, Qt::AlignCenter);
    statusRightLayout->addWidget(burnoutStateLbl, 1, 1, Qt::AlignCenter);
    statusRightLayout->addWidget(airbrakesStateLbl, 1, 2, Qt::AlignCenter);
    statusRightLayout->addWidget(apogeeStateLbl, 1, 3, Qt::AlignCenter);
    statusRightLayout->addWidget(landingStateLbl, 1, 4, Qt::AlignCenter);
    statusRightContainer->setLayout(statusRightLayout);

    armedStateLight->turnOn();
    burnoutStateLight->turnOn();
    QHBoxLayout* statusBarLayout = new QHBoxLayout;
    statusBarLayout->addWidget(logoLbl);
    statusBarLayout->addWidget(statusMidContainer);
    statusBarLayout->addWidget(statusRightContainer);
    statusBarContainer->setLayout(statusBarLayout);
}

void MainWindow::createChartViews() {
    // Setup chart and add to chartView
    altitudeChart = new RealTimeChart;
    accelerationChart = new RealTimeChart;

    altitudeChart->setXAxisTitle("Time [s]");
    altitudeChart->setYAxisTitle("Altitude [m]");
    accelerationChart->setXAxisTitle("Time [s]");
    accelerationChart->setYAxisTitle("Acceleration [m/s^2]");

    altitudeChartView = new QChartView(altitudeChart, this);
    accelerationChartView = new QChartView(accelerationChart, this);

    altitudeChartView->setRenderHint(QPainter::Antialiasing);
    altitudeChartView->setMinimumSize(
                QSize(globals::CHART_WIN_MIN_WIDTH,
                      globals::CHART_WIN_MIN_HEIGHT));

    accelerationChartView->setRenderHint(QPainter::Antialiasing);
    accelerationChartView->setMinimumSize(
                QSize(globals::CHART_WIN_MIN_WIDTH,
                      globals::CHART_WIN_MIN_HEIGHT));

    // Setup timer for updating real time plot
    timer = new QTimer;
    timer->setInterval(globals::TIMER_UPDATE);
    timer->start();
    connect(timer, &QTimer::timeout, this, &MainWindow::updateRealTimeVisuals);
}

void MainWindow::createMenuBar() {
    menuBar = new QMenuBar(this);

    addMenu = menuBar->addMenu(tr("&Add"));
    deviceMenu = addMenu->addMenu(tr("Device"));
    connect(deviceMenu, &QMenu::aboutToShow, this, &MainWindow::showAvailablePorts);

    setMenuBar(menuBar);
}

void MainWindow::createNavball() {
    QDockWidget* navballDock = new QDockWidget("Navball", this);
    navballWidget = new NavballWidget(navballDock);
    navballDock->setWidget(navballWidget);
    addDockWidget(Qt::RightDockWidgetArea, navballDock);
}

void MainWindow::createGPSMap() {
    QDockWidget* gpsMapDock = new QDockWidget("GPS map", this);

    gpsMapView = new QQuickWidget;
    gpsMapView->setSource(QUrl(QStringLiteral("qrc:/gps_map.qml")));
    gpsMapView->show();

    gpsMapDock->setWidget(gpsMapView);
    addDockWidget(Qt::RightDockWidgetArea, gpsMapDock);
}

void MainWindow::showAvailablePorts() {
    this->deviceMenu->clear();
    QStringList ports = this->serialInterface->getAvailableDevices();
    for (auto& port : ports)
        QAction* portAction = this->deviceMenu->addAction(port);
}

void MainWindow::updateRealTimeVisuals() {
    double height = 0;
    double latitude = 0;
    double longitude = 0;
    double temp = 0;
    double accY = 0;
    double timeStamp = 0;
    double altGps = 0;
    double state = 0;
    uint16_t packNum = 0;
    if (currentPort.size()) {
        double* sensorData = this->serialInterface->getSensorData();
        timeStamp = sensorData[TIMESTAMP];
        packNum = this->serialInterface->getPackageNumber();
        height = sensorData[ALTITUDE];
        altGps = sensorData[ALTITUDE_GPS];
        state = sensorData[STATE];
        latitude = sensorData[LATITUDE_GPS];
        longitude = sensorData[LONGITUDE_GPS];
        temp = sensorData[BME_TEMP];
        accY -= sensorData[ACC_Y];

        if (height > 0)
            this->altitudeChart->update(height);
        this->accelerationChart->update(accY);

        QObject* object = (gpsMapView->rootObject())->findChild<QObject*>("gpsMapItem");
        QVariant latitudeQV = QVariant(latitude);
        QVariant longitudeQV = QVariant(longitude);
        if (object != NULL) {
            QMetaObject::invokeMethod(object, "updatePosition",
                                      Q_ARG(QVariant, latitudeQV),
                                      Q_ARG(QVariant, longitudeQV));
        }
    }
}
