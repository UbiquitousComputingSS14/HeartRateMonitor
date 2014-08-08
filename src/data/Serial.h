/**
 * This class uses QtSerialPort to receive data from a light sensor.
 *
 * It parses the incoming data and emits corresponding signals for
 * sensor data and settings.
 *
 * @author Jens Gansloser
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>

namespace hrm
{

    struct SerialPortSettings {
        QString portName;
        int baudrate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;

        static SerialPortSettings getDefaultSettings() {
            SerialPortSettings settings;

            settings.portName = "COM4"; //"/dev/ttyACM0";
            settings.baudrate = 9600;
            settings.dataBits = QSerialPort::Data8;
            settings.parity = QSerialPort::NoParity;
            settings.stopBits = QSerialPort::OneStop;
            settings.flowControl = QSerialPort::NoFlowControl;

            return settings;
        }
    };

    /**
     * The data that can be received over serial.
     */
    struct SensorSettings {
        QString sensor;
        QString id;
        QString max;
        QString min;
        QString resolution;
        QString sampleInterval;
    };

    struct SensorData {
        uint16_t broadband;
        uint16_t ir;
    };

    class Serial : public QSerialPort
    {
            Q_OBJECT

        private:
            SerialPortSettings settings;
            void parseData(QString line);

        private slots:
            void receiveData();
            void handleError(QSerialPort::SerialPortError error);

        signals:
            void receiveLine(QString data);
            void receiveSensorData(SensorData data);
            void receiveSensorSettings(SensorSettings settings);

        public:
            Serial(QObject *parent = 0);
            ~Serial();

            bool openSerial();
            void closeSerial();

            void sendData(QString string);
    };

}

#endif
