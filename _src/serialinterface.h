#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>
#include <QFile>

class QSerialPort;

class SerialInterface : public QObject {
signals:
    void deviceChanged(QString /* Device Name */);
public:
    SerialInterface(QObject* parent = 0);
    ~SerialInterface();
    QStringList getAvailableDevices(); // List all available devices
    bool setupPort(QString portName, qint32 baudRate); // Setup port with given baud rate
    bool setBaudRate(qint32 baudRate);
    qint32 getBaudRate();
    double* getSensorData();
    void setFileName(QString filename);
    void setFilePath(QString filePath);
    uint16_t getPackageNumber();

private:
    void readSerial(); // Called when device sends readyRead
    QSerialPort* serialDevice; // Current serial port device
    qint32 baudRate;
    QMap<QString, qint32> deviceBaudRate; // Port name as key, baud rate as value
    QByteArray buffer; // Serial buffer

    double* sensorData; // Stores data that
    uint16_t packageNumber; // Stores current package number

    QFile *dataFile; // File object for storing all incoming raw data
    QString dataFilePath;

};

#endif // SERIALINTERFACE_H
