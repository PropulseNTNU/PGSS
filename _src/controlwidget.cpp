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

ControlWidget::ControlWidget(QWidget *parent) : QWidget(parent),
    dataPath(globals::DEFAULT_DATA_PATH),
    dataFilename(globals::DEFAULT_DATA_FILENAME)
{
    createLeftSide();
    createRightSide();
    outputList = new QListWidget;
    outputList->setSelectionMode(QAbstractItemView::NoSelection);
    QSizePolicy sizePolicy = outputList->sizePolicy();
    sizePolicy.setHorizontalStretch(5);
    outputList->setSizePolicy(sizePolicy);
    QGridLayout* centralLayout = new QGridLayout;
    centralLayout->addWidget(leftSideBox, 0, 0, 0, 1);
    centralLayout->addWidget(outputList, 0, 1, 0, 5);
    centralLayout->addWidget(rightSideBox, 0, 6, 0, 1);

    setLayout(centralLayout);
}

void ControlWidget::createLeftSide()
{
    leftFirstLineContainer = new QWidget;
    baudRateLbl = new QLabel("Baud rate:");
    baudRateLEdit = new QLineEdit(QString::number(globals::SERIAL_BAUD_RATE));
    baudRateBtn = new QPushButton("Set");
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
    deviceNameLbl = new QLabel("None");
    QHBoxLayout* secondLineLayout = new QHBoxLayout;
    secondLineLayout->addWidget(deviceLbl);
    secondLineLayout->addWidget(deviceNameLbl);
    leftSecondLineContainer->setLayout(secondLineLayout);

    leftSideBox = new QGroupBox(this);
    QVBoxLayout* leftSideBoxLayout = new QVBoxLayout;
    leftSideBoxLayout->addWidget(leftFirstLineContainer);
    leftSideBoxLayout->addWidget(leftSecondLineContainer);
    leftSideBoxLayout->setSpacing(0);
    leftSideBox->setLayout(leftSideBoxLayout);
}

void ControlWidget::createRightSide()
{

    topRightLbl = new QLabel("Data storage");

    rightFirstLineContainer = new QWidget;
    pathLbl = new QLabel("Path:");
    pathLEdit = new QLineEdit(dataPath);
    pathBtn = new QPushButton("Set");
    connect(pathBtn, &QPushButton::clicked, [this] {
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
    filenameLEdit = new QLineEdit(dataFilename);
    filenameBtn = new QPushButton("Set");
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
    rightSideBoxLayout->setSpacing(0);
    rightSideBox->setLayout(rightSideBoxLayout);
}

void ControlWidget::writeToLog(QString message)
{

}

void ControlWidget::writeToOutput(QString message)
{

}

void ControlWidget::setDeviceName(QString deviceName)
{

}