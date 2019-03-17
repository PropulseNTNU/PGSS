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
    centralwid = new QWidget;
    speed = 0;
    stringDataFile = new QFile("/Users/olebjorn/Desktop/Test_Launch_Data/string_data_launch_3.txt");
    stringDataFile->open(QIODevice::ReadWrite);
    // Create a serial interface
    serialInterface = new SerialInterface(this);

    // Create widgets
    createAltitudeChartView();
    createDeviceSelector();
   // createNavball();
    createMenuBar();
    createGPSMap();

    QVBoxLayout* lay = new QVBoxLayout;
    lay->addWidget(altitudeChartView);
    lay->addWidget(speedChartView);
    centralwid->setLayout(lay);
    // Set altitude chart as central widget
    setCentralWidget(centralwid);
}

MainWindow::~MainWindow() {

}

void MainWindow::createAltitudeChartView() {
    // Setup chart and add to chartView
    altitudeChart = new RealTimeChart;
    speedChart = new RealTimeChart;
    speedChart->setXAxisTitle("Time [s]");
    speedChart->setYAxisTitle("Acceleration [m/s^2]");
    altitudeChart->setXAxisTitle("Time [s]");
    altitudeChart->setYAxisTitle("Altitude [m]");

    speedChartView = new QChartView(speedChart, this);
    altitudeChartView = new QChartView(altitudeChart, this);

    speedChartView->setRenderHint(QPainter::Antialiasing);
    speedChartView->setMinimumSize(
                QSize(globals::CHART_WIN_MIN_WIDTH,
                      globals::CHART_WIN_MIN_HEIGHT));
    altitudeChartView->setRenderHint(QPainter::Antialiasing);
    altitudeChartView->setMinimumSize(
                QSize(globals::CHART_WIN_MIN_WIDTH,
                      globals::CHART_WIN_MIN_HEIGHT));


    // Setup timer for updating real time plot
    timer = new QTimer;
    timer->setInterval(globals::TIMER_UPDATE);
    timer->start();
    connect(timer, &QTimer::timeout, this, &MainWindow::updateSensorData);
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


void MainWindow::updateSensorData() {
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
        speed -= accY*0.1;

        if (height > 0)
            this->altitudeChart->update(height);
        this->speedChart->update(accY);

        QTextStream textStream(stringDataFile);
        textStream << "Time stamp: " << timeStamp << " ";
        textStream << "Package Number: " << packNum << " ";
        textStream << "Height: " << height << " ";
        textStream << "GPS Alt: " << altGps<< " ";
        textStream << "Latitude: " << latitude << " ";
        textStream << "Longitude " << longitude << " ";
        textStream << "BME Temp: " << temp << " ";
        textStream << "State: " << state  << "\n";
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
