#include "mainwindow.h"
#include "globals.h"
#include "realtimechart.h"
#include "serialinterface.h"
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), currentPort("") {
    // Create a serial interface
    serialInterface = new SerialInterface(this);

    // Create widgets
    createAltitudeChartView();
    createDeviceSelector();
    createNavball();
    createMenuBar();

    // Set altitude chart as central widget
    setCentralWidget(altitudeChartView);
}

MainWindow::~MainWindow() {

}

void MainWindow::createAltitudeChartView() {
    // Setup chart and add to chartView
    altitudeChart = new RealTimeChart;
    altitudeChart->setXAxisTitle("Time [s]");
    altitudeChart->setYAxisTitle("Height [cm]");

    altitudeChartView = new QChartView(altitudeChart, this);
    altitudeChartView->setRenderHint(QPainter::Antialiasing);
    altitudeChartView->setMinimumSize(
                QSize(globals::CHART_WIN_MIN_WIDTH,
                      globals::CHART_WIN_MIN_HEIGHT));


    // Setup timer for updating real time plot
    timer = new QTimer;
    timer->setInterval(globals::TIMER_UPDATE);
    timer->start();
    connect(timer, &QTimer::timeout, [this] {
        double y = 0;
        if (currentPort.size()) {
            y = this->serialInterface->getValue(currentPort);
            if (y > 0)
                this->altitudeChart->update(y);
        }
    });
}

void MainWindow::createDeviceSelector() {
    QDockWidget* deviceSelectorDock = new QDockWidget("Devices", this);

    deviceSelectorWidget = new QWidget;
    deviceListWidget = new QListWidget(this);

    baudRateBox = new QGroupBox(this);
    baudRateLbl = new QLabel("<b>Set baud rate:</b>");
    baudRateInput = new QLineEdit("9600", this);
    setBaudRateBtn = new QPushButton("Apply");
    updateDevicesBtn = new QPushButton("Update devices");

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

void MainWindow::showAvailablePorts() {
    this->deviceMenu->clear();
    QStringList ports = this->serialInterface->getAvailableDevices();
    for (auto& port : ports) {
        QAction* portAction = this->deviceMenu->addAction(port);
        connect(portAction, &QAction::triggered, [this, portAction] {
            QString portName = portAction->text();
            qDebug() << portName;
            if (this->serialInterface->setupPort(portName, 9600)) {
                qDebug() << "Connected to port";
                this->deviceListWidget->addItem(portName);
                currentPort = portName;
            }
        });
    }
}
