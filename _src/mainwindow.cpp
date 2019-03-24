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
   // createGPSMap();
    createStatusBar();
    createDataSection();
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
    centralWidget->setObjectName("centralWidget");
    controlWidget = new ControlWidget;



    QGridLayout* centralLayout = new QGridLayout;
    // QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment
    centralLayout->addWidget(statusBarContainer, 0, 0, 1, 10,  Qt::AlignLeft);
    centralLayout->addWidget(dataSectionContainer, 1, 0, 3, 4,  Qt::AlignLeft);
    centralLayout->addWidget(chartContainer, 1, 4, 3, 6, Qt::AlignCenter);
    centralLayout->addWidget(controlWidget, 4, 0, 2, 10, Qt::AlignBaseline);
    /*
    centralLayout->addWidget(statusBarContainer, 0, 0, 1, 5, Qt::AlignLeading);
    centralLayout->addWidget(topDataContainer, 1, 0, 2, 2, Qt::AlignLeading);
    centralLayout->addWidget(bottomDataContainer, 3, 0, 1, 2, Qt::AlignLeading);
    centralLayout->addWidget(altitudeChartView, 0, 5, Qt::AlignTrailing);
    centralLayout->addWidget(accelerationChartView, 1, 5, Qt::AlignTrailing); */
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
    apogeeStateLbl->setObjectName("Apogee");// gir hver enkelt en id
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

void MainWindow::createDataSection() {
    dataSectionContainer = new QWidget;

    topDataContainer = new QWidget;
    topLabel = new QLabel("Speed and location:");
    altitudeLeftLbL = new QLabel("Current Altitude [m]");
    altitudeRightLbl = new QLabel("0000");
    maxAltitudeLeftLbL = new QLabel("Max Altitude [m]");
    maxAltiudeRightLbl = new QLabel("0000");
    velocityLeftLbL = new QLabel("Current Velocity [m/s]");
    velocityRightLbl = new QLabel("000");
    maxVelocityLeftLbL = new QLabel("Max Velocity [m/s]");
    maxVelocityRightLbl = new QLabel("000");
    accelerationLeftLbL = new QLabel("Acceleration [m/s^2]");
    accelerationRightLbl = new QLabel("000");
    gpsLeftLbl = new QLabel("GPS Location (lat,long)");
    gpsMidLbl = new QLabel("00.000");
    gpsRightLbl = new QLabel("00.000");
    QGridLayout* topDataLayout = new QGridLayout;
    topDataLayout->addWidget(topLabel, 0, 0, Qt::AlignLeft);
    topDataLayout->addWidget(altitudeLeftLbL, 1, 0, Qt::AlignLeft);
    topDataLayout->addWidget(altitudeRightLbl, 1, 1, Qt::AlignLeft);
    topDataLayout->addWidget(maxAltitudeLeftLbL, 2, 0, Qt::AlignLeft);
    topDataLayout->addWidget(maxAltiudeRightLbl, 2, 1, Qt::AlignLeft);
    topDataLayout->addWidget(velocityLeftLbL, 3, 0, Qt::AlignLeft);
    topDataLayout->addWidget(velocityRightLbl, 3, 1, Qt::AlignLeft);
    topDataLayout->addWidget(maxVelocityLeftLbL, 4, 0, Qt::AlignLeft);
    topDataLayout->addWidget(maxVelocityRightLbl, 4, 1, Qt::AlignLeft);
    topDataLayout->addWidget(accelerationLeftLbL, 5, 0, Qt::AlignLeft);
    topDataLayout->addWidget(accelerationRightLbl, 5, 1, Qt::AlignLeft);
    topDataLayout->addWidget(gpsLeftLbl, 6, 0, Qt::AlignLeft);
    topDataLayout->addWidget(gpsMidLbl, 6, 1, Qt::AlignLeft);
    topDataLayout->addWidget(gpsRightLbl, 6, 2, Qt::AlignLeft);
    topDataContainer->setLayout(topDataLayout);

    bottomDataContainer = new QWidget;
    pitchLeftLbl = new QLabel("Pitch");
    pitchRightLbl = new QLabel("0000");
    rollLeftLbl = new QLabel("Roll");
    rollRightLbl = new QLabel("0000");
    yawLeftLbl = new QLabel("Yaw");
    yawRightLbl = new QLabel("0000");
    pitchRateLeftLbl = new QLabel("Pitch Rate");
    pitchRateRightLbl = new QLabel("0000");
    rollRateLeftLbl = new QLabel("Roll Rate");
    rollRateRightLbl = new QLabel("0000");
    yawRateLeftLbl = new QLabel("Yaw Rate");
    yawRateRightLbl = new QLabel("0000");
    QGridLayout* bottomDataLayout = new QGridLayout;
    bottomDataLayout->addWidget(pitchLeftLbl, 0, 0, Qt::AlignLeft);
    bottomDataLayout->addWidget(pitchRightLbl,0, 1, Qt::AlignLeft);
    bottomDataLayout->addWidget(pitchRateLeftLbl, 0, 2, Qt::AlignLeft);
    bottomDataLayout->addWidget(pitchRateRightLbl, 0, 3, Qt::AlignLeft);
    bottomDataLayout->addWidget(rollLeftLbl, 1, 0, Qt::AlignLeft);
    bottomDataLayout->addWidget(rollRightLbl, 1, 1, Qt::AlignLeft);
    bottomDataLayout->addWidget(rollRateLeftLbl, 1, 2, Qt::AlignLeft);
    bottomDataLayout->addWidget(rollRateRightLbl, 1, 3, Qt::AlignLeft);
    bottomDataLayout->addWidget(yawLeftLbl, 2, 0, Qt::AlignLeft);
    bottomDataLayout->addWidget(yawRightLbl, 2, 1, Qt::AlignLeft);
    bottomDataLayout->addWidget(yawRateLeftLbl, 2, 2, Qt::AlignLeft);
    bottomDataLayout->addWidget(yawRateRightLbl, 2, 3, Qt::AlignLeft);
    bottomDataContainer->setLayout(bottomDataLayout);

    QVBoxLayout* dataSectionLayout = new QVBoxLayout;
    dataSectionLayout->addWidget(topDataContainer);
    dataSectionLayout->addWidget(bottomDataContainer);
    dataSectionContainer->setLayout(dataSectionLayout);
}

void MainWindow::createChartViews() {
    chartContainer = new QWidget;

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

    QHBoxLayout* chartLayout = new QHBoxLayout;
    chartLayout->addWidget(altitudeChartView);
    chartLayout->addWidget(accelerationChartView);
    chartContainer->setLayout(chartLayout);
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
