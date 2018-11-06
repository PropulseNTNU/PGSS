#include "serialinterface.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>


SerialInterface::SerialInterface(QObject* parent) : buffer(""), parsedData("") {
    //device = new QSerialPort;
}


QStringList SerialInterface::getAvailableDevices() {
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QStringList portNames;
    for (auto& port : ports)
        portNames.append(port.portName());
    return portNames;
}

bool SerialInterface::setupPort(QString portName, qint32 baudRate) {
    QSerialPortInfo portInfo(portName);

    if (portInfo.isNull())
        return false;
    if (portInfo.productIdentifier() != 67)
        return false;

    QSerialPort* device = new QSerialPort(portName, this);
    device->open(QSerialPort::ReadOnly);

    // Configure (Use default values if not set)
    device->setBaudRate(baudRate);
    if (!device->setDataBits(QSerialPort::Data8)) {
        delete device;
        device = nullptr;
        return false;
    }
    connect(device, &QSerialPort::readyRead, this, &SerialInterface::readSerial);
    devices[portName] = device;
    deviceValue[portName] = "";
    return true;
}

void SerialInterface::readSerial() {
    QSerialPort* device = qobject_cast<QSerialPort*>(QObject::sender());
    QString data = device->readAll();
    data.remove('\r');
    data.remove('\n');
    if (data.size())
        deviceValue[device->portName()] = data;
}

bool SerialInterface::setBaudRate(QString portName, qint32 baudRate) {
    if (devices[portName]->setBaudRate(baudRate)) {
        deviceBaudRate[portName] = baudRate;
        return true;
    }
    return false;
}

qint32 SerialInterface::getBaudRate(QString portName) {
    return deviceBaudRate[portName];
}

double SerialInterface::getValue(QString portName) {
    return deviceValue[portName].toDouble();
}
