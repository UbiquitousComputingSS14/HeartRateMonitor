#include "Serial.h"

#include <QMessageBox>

namespace hrm
{

    Serial::Serial(QObject *parent) :
        QSerialPort(parent),
        settings(SerialPortSettings::getDefaultSettings())
    {
        connect(this, SIGNAL(readyRead()), this, SLOT(receiveData()));
        connect(this, SIGNAL(error(QSerialPort::SerialPortError)), this,
                SLOT(handleError(QSerialPort::SerialPortError)));
    }

    Serial::~Serial()
    {
    }

    void Serial::receiveData()
    {
        if (canReadLine()) {
            QByteArray line = readLine();
            line.trimmed();

            parseData(line);

            Q_EMIT receiveLine(line);
        }
    }

    void Serial::parseData(QString line)
    {
        QList<QString> dataWords = line.split(' ');

        if (dataWords.size() >= 1) {
            if (dataWords[0] == "data:") {
                if (dataWords.size() != 5)
                    return;

                if (dataWords[1] == "broadband" && dataWords[3] == "ir") {
                    SensorData data;
                    data.broadband = dataWords[2].trimmed().toUShort();
                    data.ir = dataWords[4].trimmed().toUShort();

                    Q_EMIT receiveSensorData(data);
                }
            } else if (dataWords[0] == "settings:") {
                if (dataWords.size() != 14)
                    return;

                SensorSettings settings;

                settings.sensor = dataWords[2];
                settings.id = dataWords[4];
                settings.max = dataWords[6];
                settings.min = dataWords[8];
                settings.resolution = dataWords[10];
                settings.sampleInterval = dataWords[12];

                Q_EMIT receiveSensorSettings(settings);
            }
        }
    }

    bool Serial::openSerial()
    {
        setPortName(settings.portName);
        setBaudRate(settings.baudrate);
        setDataBits(settings.dataBits);
        setParity(settings.parity);
        setStopBits(settings.stopBits);
        setFlowControl(settings.flowControl);

        if (open(QIODevice::ReadWrite))
            return true;
        return false;
    }

    void Serial::closeSerial()
    {
        close();
    }

    void Serial::sendData(QString string)
    {
        write(string.toLocal8Bit());
    }

    void Serial::handleError(QSerialPort::SerialPortError error)
    {
        if (error == QSerialPort::ResourceError) {
            QMessageBox::critical(0, tr("Critical Error"), errorString());
            closeSerial();
        }
    }

}
