#include "controlwidget.h"
#include "globals.h"
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

ControlWidget::ControlWidget(QWidget *parent) : QWidget(parent),
    dataPath(globals::DEFAULT_DATA_PATH),
    dataFilename(globals::DEFAULT_DATA_FILENAME)
{
    logFile = new QFile(dataPath+"log_"+dataFilename);

    createLeftSide();
    createRightSide();
    outputList = new QListWidget;
    outputList->setObjectName("Terminal");

    outputList->setSelectionMode(QAbstractItemView::NoSelection);
    QSizePolicy sizePolicy = outputList->sizePolicy();
    sizePolicy.setHorizontalStretch(5);
    outputList->setSizePolicy(sizePolicy);

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(leftSideBox, 0, 0, 0, 1);
    gridLayout->addWidget(outputList, 0, 1, 0, 5);
    gridLayout->addWidget(rightSideBox, 0, 6, 0, 1);

    QVBoxLayout* centralLayout = new QVBoxLayout;
    centralLayout->addLayout(gridLayout);
    setLayout(centralLayout);
}

ControlWidget::~ControlWidget() {
    if (logFile)
        logFile->close();
        delete logFile;
}

void ControlWidget::createLeftSide()
{
    leftFirstLineContainer = new QWidget;
    baudRateLbl = new QLabel("Baud rate:");
    baudRateLbl->setObjectName("BaudRate");
    baudRateLEdit = new QLineEdit(QString::number(globals::SERIAL_BAUD_RATE));
    baudRateBtn = new QPushButton("Apply");
    baudRateBtn->setObjectName("Set");
    connect(baudRateBtn, &QPushButton::clicked, [this] {
        emit this->baudRateChanged(this->baudRateLEdit->text().toInt());
    });
    QHBoxLayout* firstLineLayout = new QHBoxLayout;
    firstLineLayout->addWidget(baudRateLbl);
    firstLineLayout->addWidget(baudRateLEdit);
    firstLineLayout->addWidget(baudRateBtn);
    leftFirstLineContainer->setLayout(firstLineLayout);

    leftSecondLineContainer = new QWidget;
    deviceLbl = new QLabel("Device:");
    deviceLbl->setObjectName("Device");
    deviceNameLbl = new QLabel("None");
    deviceNameLbl->setObjectName("None");
    QHBoxLayout* secondLineLayout = new QHBoxLayout;
    secondLineLayout->addWidget(deviceLbl);
    secondLineLayout->addWidget(deviceNameLbl);
    leftSecondLineContainer->setLayout(secondLineLayout);

    leftSideBox = new QGroupBox(this);
    QVBoxLayout* leftSideBoxLayout = new QVBoxLayout;
    leftSideBoxLayout->addWidget(leftFirstLineContainer);
    leftSideBoxLayout->addWidget(leftSecondLineContainer);

    leftSideBox->setLayout(leftSideBoxLayout);
}

void ControlWidget::createRightSide()
{

    topRightLbl = new QLabel("Data storage");
    topRightLbl->setObjectName("DataStorage");

    rightFirstLineContainer = new QWidget;
    pathLbl = new QLabel("Path:");
    pathLbl->setObjectName("Path");
    pathLEdit = new QLineEdit(dataPath);
    pathBtn = new QPushButton("Set");
    pathBtn->setObjectName("Set");
    connect(pathBtn, &QPushButton::clicked, [this] {

         QString newPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      this->dataPath,
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks) + "/";
         if (newPath != "/")
            this->dataPath = newPath;
         if (this->logFile) {
            this->logFile->close();
            delete this->logFile;
            this->logFile = nullptr;
        }
        this->logFile = new QFile(this->dataPath+"log_"+this->dataFilename);
        this->logFile->open(QIODevice::ReadWrite);
        this->pathLEdit->setText(this->dataPath);
        emit this->filenameChanged(this->dataPath+this->dataFilename);
    });
    QHBoxLayout* firstLineLayout = new QHBoxLayout;
    firstLineLayout->addWidget(pathLbl);
    firstLineLayout->addWidget(pathLEdit);
    firstLineLayout->addWidget(pathBtn);
    rightFirstLineContainer->setLayout(firstLineLayout);

    // Second line
    rightSecondLineContainer = new QWidget;
    filenameLbl = new QLabel("Filename:");
    filenameLbl->setObjectName("Filename");
    filenameLEdit = new QLineEdit(dataFilename);
    filenameBtn = new QPushButton("Set");
    filenameBtn->setObjectName("Set");
    connect(filenameBtn, &QPushButton::clicked, [this] {
        this->dataFilename = filenameLEdit->text();
        if (this->logFile) {
            this->logFile->close();
            delete this->logFile;
            this->logFile = nullptr;
        }
        this->logFile = new QFile(this->dataPath+"log_"+this->dataFilename);
        this->logFile->open(QIODevice::ReadWrite);
        emit this->filenameChanged(this->dataPath+this->dataFilename);
    });
    QHBoxLayout* rightSecondLineContainerLayout = new QHBoxLayout;
    rightSecondLineContainerLayout->addWidget(filenameLbl);
    rightSecondLineContainerLayout->addWidget(filenameLEdit);
    rightSecondLineContainerLayout->addWidget(filenameBtn);
    rightSecondLineContainer->setLayout(rightSecondLineContainerLayout);

    rightSideBox = new QGroupBox;
    QVBoxLayout* rightSideBoxLayout = new QVBoxLayout;
    rightSideBoxLayout->addWidget(topRightLbl);
    rightSideBoxLayout->addWidget(rightFirstLineContainer);
    rightSideBoxLayout->addWidget(rightSecondLineContainer);
    rightSideBox->setLayout(rightSideBoxLayout);
}

void ControlWidget::writeToOutput(QString message)
{
    QTextStream stream(logFile);
    stream << message << "\n";
    outputList->addItem(message);
}

void ControlWidget::setDeviceName(QString deviceName)
{
    deviceNameLbl->setText(deviceName);
}

qint32 ControlWidget::getBaudRate()
{
    return this->baudRateLEdit->text().toInt();
}
