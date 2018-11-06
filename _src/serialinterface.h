#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <QObject>
#include <QString>
#include <QMap>

class QSerialPort;

class SerialInterface : public QObject {
public:
    SerialInterface(QObject* parent = 0);
    QStringList getAvailableDevices(); // List all available devices
    bool setupPort(QString portName, qint32 baudRate); // Setup port with given baud rate
    bool setBaudRate(QString portName, qint32 baudRate);
    qint32 getBaudRate(QString portName);
    double getValue(QString portName);

private:
    void readSerial(); // Called when device sends readyRead
    QMap<QString, QSerialPort*> devices; // Port name as key, QSerialPort object as value
    QMap<QString, QString> deviceValue; // Port name as key, current value as value
    QMap<QString, qint32> deviceBaudRate; // Port name as key, baud rate as value
    QString buffer;
    QString parsedData;

};

#endif // SERIALINTERFACE_H
