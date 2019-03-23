#include "mainwindow.h"
#include "globals.h"
#include "realtimechart.h"
#include "serialinterface.h"
#include "xbee.h"
#include "navballwidget.h"

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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), currentPort("") {
    centralWidget = new QWidget;

    // Create a serial interface
    serialInterface = new SerialInterface(this);

    // Create widgets
    createChartViews();
    createDeviceSelector();
   // createNavball();
    createMenuBar();
    createGPSMap();
    createCentralWidget();

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {

}

void MainWindow::createCentralWidget() {
    QVBoxLayout* centralLayout = new QVBoxLayout;
    centralLayout->addWidget(altitudeChartView);
    centralLayout->addWidget(accelerationChartView);
    centralWidget->setLayout(centralLayout);
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

void MainWindow::createDeviceSelector() {
    QDockWidget* deviceSelectorDock = new QDockWidget("Devices", this);

    deviceSelectorWidget = new QWidget;
    deviceListWidget = new QListWidget(this);

    baudRateBox = new QGroupBox(this);
    baudRateLbl = new QLabel("<b>Set filename:</b>");
    baudRateInput = new QLineEdit("rocket_data.txt", this);
    setBaudRateBtn = new QPushButton("Apply");
    updateDevicesBtn = new QPushButton("Update devices");

    connect(setBaudRateBtn, &QPushButton::clicked, [this] {
           this->serialInterface->setFileName(baudRateInput->text());
    });
    connect(updateDevicesBtn, &QPushButton::clicked, [this] {
        this->deviceListWidget->clear();
        this->deviceListWidget->addItems(
                    this->serialInterface->getAvailableDevices());
    });
    QVBoxLayout* bottomPanelLayout = new QVBoxLayout;
    QHBoxLayout* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(baudRateInput);
    inputLayout->addWidget(setBaudRateBtn);
    bottomPanelLayout->addWidget(baudRateLbl);
    bottomPanelLayout->addLayout(inputLayout);
    bottomPanelLayout->addWidget(updateDevicesBtn);
    bottomPanelLayout->setSpacing(0);
    baudRateBox->setLayout(bottomPanelLayout);

    QVBoxLayout* deviceSelectorLayout = new QVBoxLayout;
    deviceSelectorLayout->addWidget(deviceListWidget);
    deviceSelectorLayout->addWidget(baudRateBox);
    deviceSelectorWidget->setLayout(deviceSelectorLayout);

    deviceSelectorDock->setWidget(deviceSelectorWidget);
    addDockWidget(Qt::LeftDockWidgetArea, deviceSelectorDock);
}

void MainWindow::createMenuBar() {
    menuBar = new QMenuBar(this);

    addMenu = menuBar->addMenu(tr("&Add"));
    deviceMenu = addMenu->addMenu(tr("Device"));
    connect(deviceMenu, &QMenu::aboutToShow, this, &MainWindow::showAvailablePorts);
    //connect(deviceMenu, &QMenu::aboutToHide, [this] {
     //  this->deviceMenu->clear();
    //});

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
    for (auto& port : ports) {
        QAction* portAction = this->deviceMenu->addAction(port);
        connect(portAction, &QAction::triggered, [this, portAction] {
            QString portName = portAction->text();
            if (this->serialInterface->setupPort(portName, 115200)) {
                this->deviceListWidget->addItem(portName);
                currentPort = portName;
            }
        });
    }
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
