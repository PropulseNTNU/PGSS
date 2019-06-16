#include "mainwindow.h"
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
    QMainWindow(parent), currentPort(""),
    maxAltitude(0), prevPitch(0), prevRoll(0),
    prevYaw(0)
{
    // Create a serial interface
    serialInterface = new SerialInterface(this);

    // Create widgets
    createChartViews();
    createMenuBar();
    createGPSMap();
    createStatusBar();
    createDataSection();
    createControlWidget();
    createCentralWidget();

    // Setup timer for updating real time plot
    timer = new QTimer;
    timer->setInterval(globals::TIMER_UPDATE);
    timer->start();
    connect(timer, &QTimer::timeout, this, &MainWindow::updateRealTimeVisuals);

    // Set central widget
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete serialInterface;
    delete timer;
}

void MainWindow::createCentralWidget()
{
    centralWidget = new QWidget;
    centralWidget->setObjectName("centralWidget");
    QGridLayout* centralLayout = new QGridLayout;
    centralLayout->addWidget(statusBarContainer, 0, 0, 1, 10,  Qt::AlignLeft);
    centralLayout->addWidget(dataSectionContainer, 1, 0, 3, 4,  Qt::AlignLeft);
    centralLayout->addWidget(chartContainer, 1, 4, 4, 6);
    centralLayout->addWidget(controlWidget, 5, 0, 1, 10, Qt::AlignBottom);
    centralLayout->setVerticalSpacing(0);
    centralWidget->setLayout(centralLayout);
}

void MainWindow::createStatusBar()
{
    statusBarContainer = new QWidget;

    logoLbl = new QLabel;
    logoPixmap = new QPixmap(":/propulse_logo.png");
    logoLbl->setPixmap(*logoPixmap);
    logoLbl->setScaledContents(true);
    logoLbl->setMaximumSize(QSize(globals::STATUSBAR_LOGO_WIDTH,
                                  globals::STATUSBAR_LOGO_HEIGHT));

    statusMidContainer = new QWidget;
    missionTimeLbl = new QLabel("Mission Time:");
    missionTimeLbl->setObjectName("mTimeLabel");
    timeLbl = new QLabel("00:00:00:00");
    timeLbl->setObjectName("timeLabel");
    timeExpLbl = new QLabel("HH:MM:SS:MS");
    timeExpLbl->setObjectName("timeLabel");
    QVBoxLayout* statusMidLayout = new QVBoxLayout;
    statusMidLayout->addWidget(missionTimeLbl, Qt::AlignLeft);
    statusMidLayout->addWidget(timeLbl, Qt::AlignLeft);
    statusMidLayout->addWidget(timeExpLbl, Qt::AlignLeft);
    statusMidLayout->insertSpacing(0, 100);
    statusMidLayout->insertSpacing(4, 100);
    statusMidContainer->setLayout(statusMidLayout);

    statusRightContainer = new QWidget;

    armedStateLbl = new QLabel("ARMED");
    armedStateLbl->setObjectName("descriptionLabel");
    burnoutStateLbl = new QLabel("BURNOUT");
    burnoutStateLbl->setObjectName("descriptionLabel");
    airbrakesStateLbl = new QLabel("BRAKES");
    airbrakesStateLbl->setObjectName("descriptionLabel");
    apogeeStateLbl = new QLabel("APOGEE");
    apogeeStateLbl->setObjectName("descriptionLabel");
    drogueStateLbl = new QLabel("DROGUE");
    drogueStateLbl->setObjectName("descriptionLabel");
    chuteStateLbl = new QLabel("MAIN");
    chuteStateLbl->setObjectName("descriptionLabel");
    landedStateLbl = new QLabel("LANDED");
    landedStateLbl->setObjectName("descriptionLabel");

    armedStateLight = new LightWidget(QColor(Qt::green));
    burnoutStateLight = new LightWidget(QColor(Qt::green));
    airbrakesStateLight = new LightWidget(QColor(Qt::green));
    apogeeStateLight = new LightWidget(QColor(Qt::green));
    drogueStateLight = new LightWidget(QColor(Qt::green));
    chuteStateLight = new LightWidget(QColor(Qt::green));
    landedStateLight = new LightWidget(QColor(Qt::green));
    QGridLayout* statusRightGridLayout = new QGridLayout;
    statusRightGridLayout->addWidget(armedStateLight, 0, 0, Qt::AlignCenter);
    statusRightGridLayout->addWidget(burnoutStateLight, 0, 1, Qt::AlignCenter);
    statusRightGridLayout->addWidget(airbrakesStateLight, 0, 2, Qt::AlignCenter);
    statusRightGridLayout->addWidget(apogeeStateLight, 0, 3, Qt::AlignCenter);
    statusRightGridLayout->addWidget(drogueStateLight, 0, 4, Qt::AlignCenter);
    statusRightGridLayout->addWidget(chuteStateLight, 0, 5, Qt::AlignCenter);
    statusRightGridLayout->addWidget(landedStateLight, 0, 6, Qt::AlignCenter);
    statusRightGridLayout->addWidget(armedStateLbl, 1, 0, Qt::AlignCenter);
    statusRightGridLayout->addWidget(burnoutStateLbl, 1, 1, Qt::AlignCenter);
    statusRightGridLayout->addWidget(airbrakesStateLbl, 1, 2, Qt::AlignCenter);
    statusRightGridLayout->addWidget(apogeeStateLbl, 1, 3, Qt::AlignCenter);
    statusRightGridLayout->addWidget(drogueStateLbl, 1, 4, Qt::AlignCenter);
    statusRightGridLayout->addWidget(chuteStateLbl, 1, 5, Qt::AlignCenter);
    statusRightGridLayout->addWidget(landedStateLbl, 1, 6, Qt::AlignCenter);

    QVBoxLayout* statusRightLayout = new QVBoxLayout;
    statusRightLayout->addLayout(statusRightGridLayout);
    statusRightLayout->insertSpacing(0, 100);
    statusRightLayout->insertSpacing(2, 100);
    statusRightContainer->setLayout(statusRightLayout);

    QHBoxLayout* statusBarLayout = new QHBoxLayout;
    statusBarLayout->addWidget(logoLbl);
    statusBarLayout->addWidget(statusMidContainer);
    statusBarLayout->addWidget(statusRightContainer);
    statusBarLayout->setSpacing(0);
    statusBarContainer->setLayout(statusBarLayout);
}

void MainWindow::createDataSection()
{
    dataSectionContainer = new QWidget;

    topDataContainer = new QWidget;
    topDataLabel = new QLabel("Speed and location:");
    topDataLabel->setObjectName("topDataLabel");
    altitudeLeftLbL = new QLabel("Current Altitude [m]");
    altitudeLeftLbL->setObjectName("descriptionLabel");
    altitudeRightLbl = new QLabel("0000");
    altitudeRightLbl->setObjectName("dataLabel");
    maxAltitudeLeftLbL = new QLabel("Max Altitude [m]");
    maxAltitudeLeftLbL->setObjectName("MdescriptionLabel");
    maxAltiudeRightLbl = new QLabel("0000");
    maxAltiudeRightLbl->setObjectName("MdataLabel");
    velocityLeftLbL = new QLabel("Current Velocity [m/s]");
    velocityLeftLbL->setObjectName("descriptionLabel");
    velocityRightLbl = new QLabel("000");
    velocityRightLbl->setObjectName("dataLabel");
    maxVelocityLeftLbL = new QLabel("Max Velocity [m/s]");
    maxVelocityLeftLbL->setObjectName("descriptionLabel");
    maxVelocityRightLbl = new QLabel("000");
    maxVelocityRightLbl->setObjectName("dataLabel");
    accelerationLeftLbL = new QLabel("Acceleration [m/s^2]");
    accelerationLeftLbL->setObjectName("descriptionLabel");
    accelerationRightLbl = new QLabel("000");
    accelerationRightLbl->setObjectName("dataLabel");
    gpsLeftLbl = new QLabel("GPS Location (lat,long)");
    gpsLeftLbl->setObjectName("descriptionLabel");
    gpsMidLbl = new QLabel("00");
    gpsMidLbl->setObjectName("dataLabel");
    gpsRightLbl = new QLabel("00");
    gpsRightLbl->setObjectName("dataLabel");
    QGridLayout* topDataLayout = new QGridLayout;
    topDataLayout->addWidget(topDataLabel, 0, 0, Qt::AlignLeft);
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
    bottomDataLabel = new QLabel("Orientation:");
    bottomDataLabel->setObjectName("bottomDataLabel");
    pitchLeftLbl = new QLabel("Pitch");
    pitchLeftLbl->setObjectName("descriptionLabel");
    pitchRightLbl = new QLabel("0000");
    pitchRightLbl->setObjectName("dataLabel");
    rollLeftLbl = new QLabel("Roll");
    rollLeftLbl->setObjectName("descriptionLabel");
    rollRightLbl = new QLabel("0000");
    rollRightLbl->setObjectName("dataLabel");
    yawLeftLbl = new QLabel("Yaw");
    yawLeftLbl->setObjectName("descriptionLabel");
    yawRightLbl = new QLabel("0000");
    yawRightLbl->setObjectName("dataLabel");
    pitchRateLeftLbl = new QLabel("Pitch Rate");
    pitchRateLeftLbl->setObjectName("descriptionLabel");
    pitchRateRightLbl = new QLabel("0000");
    pitchRateRightLbl->setObjectName("dataLabel");
    rollRateLeftLbl = new QLabel("Roll Rate");
    rollRateLeftLbl->setObjectName("descriptionLabel");
    rollRateRightLbl = new QLabel("0000");
    rollRateRightLbl->setObjectName("dataLabel");
    yawRateLeftLbl = new QLabel("Yaw Rate");
    yawRateLeftLbl->setObjectName("descriptionLabel");
    yawRateRightLbl = new QLabel("0000");
    yawRateRightLbl->setObjectName("dataLabel");
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

void MainWindow::createChartViews()
{
    chartContainer = new QWidget;
    QSizePolicy sizePol = chartContainer->sizePolicy();
    sizePol.setHorizontalStretch(2);
    chartContainer->setSizePolicy(sizePol);

    // Setup chart and add to chartView
    altitudeChart = new RealTimeChart;
    altitudeChart->setYAxisRange(0,
                                 globals::ALTITUDE_CHART_YRANGE);
    QSizePolicy sizePolAlt = altitudeChart->sizePolicy();

    accelerationChart = new RealTimeChart;
    accelerationChart->setYAxisRange(
                globals::ACCELERATION_CHAR_YRANGE_NEGATIVE,
                globals::ACCELERATION_CHAR_YRANGE_POSITIVE
                );
    QSizePolicy sizePolAccel = altitudeChart->sizePolicy();

    altitudeChart->setXAxisTitle("Time [s]");
    altitudeChart->setYAxisTitle("Altitude [m]");
    accelerationChart->setXAxisTitle("Time [s]");
    accelerationChart->setYAxisTitle("Acceleration [m/s^2]");

    altitudeChartView = new QChartView(altitudeChart, this);
    accelerationChartView = new QChartView(accelerationChart, this);

    altitudeChartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout* chartLayout = new QHBoxLayout;
    chartLayout->addWidget(altitudeChartView);
    chartLayout->addWidget(accelerationChartView);
    chartContainer->setLayout(chartLayout);
}

void MainWindow::createMenuBar()
{
    menuBar = new QMenuBar(this);

    addMenu = menuBar->addMenu(tr("&Add"));
    deviceMenu = addMenu->addMenu(tr("Device"));
    connect(deviceMenu, &QMenu::aboutToShow, this, &MainWindow::showAvailablePorts);

    settingsMenu = menuBar->addMenu(tr("&Settings"));
    QAction* resetAction = settingsMenu->addAction(tr("&Reset visuals"));
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetVisuals);
    settingsMenu->addSeparator();
    QAction* startUpdatingVisualsAction = settingsMenu->addAction(tr("&Start visuals"));
    connect(startUpdatingVisualsAction, &QAction::triggered, [this] {
       this->timer->start();
    });
    QAction* stopUpdatingVisualsAction = settingsMenu->addAction(tr("&Pause visuals"));
    connect(stopUpdatingVisualsAction, &QAction::triggered, [this] {
       this->timer->stop();
    });

    viewMenu = menuBar->addMenu(tr("&View"));


    setMenuBar(menuBar);
}

void MainWindow::createNavball()
{
    QDockWidget* navballDock = new QDockWidget("Navball", this);
    navballWidget = new NavballWidget(navballDock);
    navballDock->setWidget(navballWidget);
    addDockWidget(Qt::RightDockWidgetArea, navballDock);
}

void MainWindow::createGPSMap()
{
    gpsMapView = new QQuickWidget;
    gpsMapView->setSource(QUrl(QStringLiteral("qrc:/gps_map.qml"))); 
}

void MainWindow::createControlWidget()
{
    controlWidget = new ControlWidget;
     connect(controlWidget, &ControlWidget::baudRateChanged, [this] (qint32 baudRate) {
                this->serialInterface->setBaudRate(baudRate);
            });
    connect(controlWidget, &ControlWidget::filenameChanged, [this] (QString filename) {
       this->serialInterface->setFileName(filename);
    });
    connect(serialInterface, &SerialInterface::deviceChanged, [this] (QString deviceName) {
        this->controlWidget->setDeviceName(deviceName);
    });
    connect(serialInterface, &SerialInterface::message, [this] (QString message) {
       this->controlWidget->writeToOutput(message);
    });
}

void MainWindow::showAvailablePorts()
{
    this->deviceMenu->clear();
    QStringList ports = this->serialInterface->getAvailableDevices();
    for (auto& port : ports) {
        QAction* portAction = this->deviceMenu->addAction(port);
        connect(portAction, &QAction::triggered, [this, portAction] {
           currentPort = portAction->text();
           if (serialInterface->setupPort(currentPort,
                                          this->controlWidget->getBaudRate()))
               controlWidget->writeToOutput("Device successfully set up.");
           else
               controlWidget->writeToOutput("Device not connected.");
        });
    }
}

void MainWindow::updateRealTimeVisuals()
{
    if (!currentPort.size())
        return;
    float* data = this->serialInterface->getSensorData();

    updateStateVisual((globals::state) data[STATE]);


    if (data[ALTITUDE] > maxAltitude) {
        maxAltitude = data[ALTITUDE];
        maxAltiudeRightLbl->setText(QString::number(maxAltitude, 'f', 1));
    }

    altitudeRightLbl->setText(QString::number(data[ALTITUDE], 'f', 1));
    accelerationRightLbl->setText(QString::number(data[ACC_Y], 'f', 1));
    gpsMidLbl->setText(QString::number(data[LATITUDE_GPS], 'f', 3));
    gpsRightLbl->setText(QString::number(data[LONGITUDE_GPS], 'f', 3));
    pitchRightLbl->setText(QString::number(data[PITCH], 'f', 1));
    rollRightLbl->setText(QString::number(data[ROLL])), 'f', 1;
    yawRightLbl->setText(QString::number(data[YAW],'f', 1));
    pitchRateRightLbl->setText(QString::number(data[PITCH]-prevPitch, 'f', 1));
    rollRateRightLbl->setText(QString::number(data[ROLL]-prevRoll, 'f', 1));
    yawRateRightLbl->setText(QString::number(data[YAW]-prevYaw, 'f', 1));

    if (data[ALTITUDE] > 0)
        this->altitudeChart->update(data[ALTITUDE]);
    this->accelerationChart->update(data[ACC_Y]);
}

void MainWindow::updateStateVisual(globals::state state) {
    switch (state) {
        case globals::state::LANDED:
            landedStateLight->turnOn();
        case globals::state::CHUTE:
            chuteStateLight->turnOn();
        case globals::state::DROGUE:
            drogueStateLight->turnOn();
        case globals::state::APOGEE:
            apogeeStateLight->turnOn();
        case globals::state::AIRBRAKES:
            airbrakesStateLight->turnOn();
        case globals::state::BURNOUT:
            burnoutStateLight->turnOn();
        case globals::state::ARMED:
            armedStateLight->turnOn();
        default:
            break;
    }
}

void MainWindow::resetVisuals() {
    altitudeChart->reset();
    altitudeChart->setYAxisRange(0, globals::ALTITUDE_CHART_YRANGE);
    accelerationChart->reset();
    accelerationChart->setYAxisRange(
                globals::ACCELERATION_CHAR_YRANGE_NEGATIVE,
                globals::ACCELERATION_CHAR_YRANGE_POSITIVE
                );
    armedStateLight->turnOff();
    burnoutStateLight->turnOff();
    airbrakesStateLight->turnOff();
    apogeeStateLight->turnOff();
    drogueStateLight->turnOff();
    chuteStateLight->turnOff();
    landedStateLight->turnOff();

    maxAltitude = 0;
    maxAltiudeRightLbl->setText("0000");

    altitudeRightLbl->setText("0000");
    accelerationRightLbl->setText("000");
    gpsMidLbl->setText("00");
    gpsRightLbl->setText("00");
    pitchRightLbl->setText("0000");
    rollRightLbl->setText("0000");
    yawRightLbl->setText("0000");
    pitchRateRightLbl->setText("0000");
    rollRateRightLbl->setText("0000");
    yawRateRightLbl->setText("0000");

}
