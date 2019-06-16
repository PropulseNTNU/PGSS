#include "serialinterface.h"
#include "globals.h"
#include "xbee.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QDateTime>


SerialInterface::SerialInterface(QObject* parent) : QObject(parent),
    buffer(""),
    baudRate(globals::SERIAL_BAUD_RATE),
    dataFilePath(globals::DEFAULT_DATA_PATH),
    packageNumber(0)
{
    sensorData = new float[NUM_TYPES] {};
    serialDevice = nullptr;
    dataFile = new QFile(dataFilePath+"launch_data.txt");
    if (!dataFile->open(QIODevice::ReadWrite))
        qDebug() << "Could not open:";
        // WRITE TO LOG HERE
}

SerialInterface::~SerialInterface()
{
    if (serialDevice) {
        serialDevice->clear();
        serialDevice->close();
        delete serialDevice;
        serialDevice = nullptr;
    }
    dataFile->close();
    delete dataFile;
    dataFile = nullptr;
}

QStringList SerialInterface::getAvailableDevices()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QStringList portNames;
    for (auto& port : ports)
        portNames.append(port.portName());
    return portNames;
}

bool SerialInterface::setupPort(QString portName, qint32 baudRate)
{
    if (serialDevice) {
        serialDevice->clear();
        serialDevice->close();
        delete serialDevice;
        serialDevice = nullptr;
    }

    QSerialPortInfo portInfo(portName);
    if (portInfo.isNull()) {
        emit errorMessage("..No port named: " + portName);
        return false;
    }
    QSerialPort* device = new QSerialPort(portName);
    if (!device->open(QSerialPort::ReadWrite)) {
        emit errorMessage("..Failed to open device.");
        emit errorMessage("Errorcode: " +
                          QString::number(device->error()));
        delete device;
        device = nullptr;
        return false;
    }

    // Configure (Use default values if not set)
    if (!device->setBaudRate(baudRate)) {
        emit errorMessage("..Failed to set baud rate of device.");
        emit errorMessage("Errorcode: " +
                          QString::number(device->error()));
        device->clear();
        device->close();
        delete device;
        device = nullptr;
        return false;
    }

    if (!device->setDataBits(QSerialPort::Data8)) {
        emit errorMessage("Failed to set data bits of port.");
        emit errorMessage("Errorcode: " +
                          QString::number(device->error()));
        device->clear();
        device->close();
        delete device;
        device = nullptr;
        return false;
    }

    serialDevice = device;
    serialDevice->clear();
    serialDevice->flush();
    emit deviceChanged(portName);
    connect(serialDevice, &QSerialPort::readyRead, this, &SerialInterface::readSerial);
    return true;
}

void SerialInterface::readSerial()
{
    qDebug() << "Read!";
    QByteArray data = serialDevice->readAll(); 
    QTextStream stream(dataFile);
    stream << data;
    buffer += data;
    if (buffer.size() > globals::SERIAL_BUFFER_SIZE)
        buffer.clear();
    read_buffer(buffer, (uint8_t*)sensorData,
                (NUM_TYPES)*sizeof(float), &packageNumber);
    serialDevice->clear();

}

bool SerialInterface::setBaudRate(qint32 baudRate)
{
    if (serialDevice)
        return serialDevice->setBaudRate(baudRate);
    return false;
}

qint32 SerialInterface::getBaudRate()
{
    return baudRate;
}

float* SerialInterface::getSensorData()
{
    return sensorData;
}

uint16_t SerialInterface::getPackageNumber()
{
    return packageNumber;
}

void SerialInterface::setFileName(QString filename)
{
    if (dataFile) {
        dataFile->close();
        delete dataFile;
    }
    dataFile = new QFile(dataFilePath+filename);
    dataFile->open(QIODevice::ReadWrite);
}

void SerialInterface::setFilePath(QString filePath)
{
    dataFilePath = filePath;
}
