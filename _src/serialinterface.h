#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>
#include <QFile>

class QSerialPort;

class SerialInterface : public QObject {
public:
    SerialInterface(QObject* parent = 0);
    ~SerialInterface();
    QStringList getAvailableDevices(); // List all available devices
    bool setupPort(QString portName, qint32 baudRate); // Setup port with given baud rate
    bool setBaudRate(QString portName, qint32 baudRate);
    qint32 getBaudRate(QString portName);
    double getValue(QString portName);
    double* getSensorData();
    void setFileName(QString filename);
    uint16_t getPackageNumber();

private:
    void readSerial(); // Called when device sends readyRead
    QMap<QString, QSerialPort*> devices; // Port name as key, QSerialPort object as value
    QMap<QString, QString> deviceValue; // Port name as key, current value as value
    QMap<QString, qint32> deviceBaudRate; // Port name as key, baud rate as value
    QByteArray buffer;
    QString parsedData;
    QString fileLoc = "/Users/olebjorn/Desktop/Test_Launch_Data/";
    double* sensorData;
    uint16_t packageNumber;
    QFile *dataFile;

};

#endif // SERIALINTERFACE_H
