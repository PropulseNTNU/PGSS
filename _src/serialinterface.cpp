#include "serialinterface.h"
#include "xbee.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>


SerialInterface::SerialInterface(QObject* parent) : buffer(""), parsedData("") {
    //device = new QSerialPort;
    buffSize = 300;
    sensorData = new double[NUM_TYPES];
    buff = new char[buffSize] {};
    packageNumber = 0;
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
    QSerialPort* device = new QSerialPort(portName, this);
    device->open(QSerialPort::ReadOnly);

    // Configure (Use default values if not set)
    device->setBaudRate(baudRate);
    if (!device->setDataBits(QSerialPort::Data8)) {
        qDebug() << "Could not set up port..";
        delete device;
        device = nullptr;
        return false;
    }
    qDebug() << "Port ready!";
    connect(device, &QSerialPort::readyRead, this, &SerialInterface::readSerial);
    devices[portName] = device;
    deviceValue[portName] = "";
    return true;
}

void SerialInterface::readSerial() {
    QSerialPort* device = qobject_cast<QSerialPort*>(QObject::sender());
    QByteArray data = device->readAll();
    read_buffer(data, (uint8_t*)sensorData, NUM_TYPES*sizeof(double),
                       &packageNumber);
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

double* SerialInterface::getSensorData() {
    return sensorData;
}
