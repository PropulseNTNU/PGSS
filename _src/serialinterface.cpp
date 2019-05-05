#include "serialinterface.h"
#include "globals.h"
#include "xbee.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QDateTime>



SerialInterface::SerialInterface(QObject* parent) : buffer(""),
    baudRate(globals::SERIAL_BAUD_RATE), dataFilePath(""),
    packageNumber(0) {
    sensorData = new double[NUM_TYPES] {};
    serialDevice = nullptr;
    dataFile = new QFile(dataFilePath+"launch_data.txt");
    if (!dataFile->open(QIODevice::ReadWrite))
        qDebug() << "Could not open:";
        // WRITE TO LOG HERE
}

SerialInterface::~SerialInterface() {
    dataFile->close();
    delete dataFile;
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

    QSerialPort* device = new QSerialPort(portName, this);
    device->open(QSerialPort::ReadOnly);

    // Configure (Use default values if not set)
    device->setBaudRate(baudRate);

    if (!device->setDataBits(QSerialPort::Data8)) {
        // WRITE TO LOG HERE
        delete device;
        device = nullptr;
        return false;
    }

    serialDevice = device;
    //emit deviceChanged(portName);
    connect(device, &QSerialPort::readyRead, this, &SerialInterface::readSerial);
    return true;
}

void SerialInterface::readSerial() {
    QByteArray data = serialDevice->readAll(); 
    QTextStream stream(dataFile);
    stream << data;
    buffer += data;
    if (buffer.size() > globals::SERIAL_BUFFER_SIZE)
        buffer.clear();
    read_buffer(buffer, (uint8_t*)sensorData,
                (NUM_TYPES)*sizeof(double), &packageNumber);

}

bool SerialInterface::setBaudRate(qint32 baudRate) {
    return serialDevice->setBaudRate(baudRate);
}

qint32 SerialInterface::getBaudRate() {
    return baudRate;
}

double* SerialInterface::getSensorData() {
    return sensorData;
}

uint16_t SerialInterface::getPackageNumber() {
    return packageNumber;
}

void SerialInterface::setFileName(QString filename) {
    if (dataFile) {
        dataFile->close();
        delete dataFile;
    }
    dataFile = new QFile(dataFilePath+filename);
    dataFile->open(QIODevice::ReadWrite);
}

void SerialInterface::setFilePath(QString filePath) {
    dataFilePath = filePath;
}
