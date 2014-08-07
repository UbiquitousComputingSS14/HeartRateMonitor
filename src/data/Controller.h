/**
 * This class interfaces between Qt and the FFT classes.
 *
 * [Serial] <=> [Controller] <=> [FFT, FFTBuffer]
 * [Controller] <=> [GUI]
 *
 * Qts slot and signal mechanism is used for communication
 * between controller and GUI.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "FFT.h"
#include "Serial.h"

#include <QObject>

namespace hrm
{

    class Controller : public QObject
    {
            Q_OBJECT

        private:
            static const QString GET_SETTINGS_SERIAL;
            static const QString SET_SAMPLE_INTERVAL_SERIAL;

            Serial *serial;
            FFT fft;

            void initSignals();

        private slots:
            void receiveSensorData(SensorData data);
            void receiveSensorSettings(SensorSettings settings);

        signals:
            void sensorSettings(
                SensorSettings settings,
                FFT_properties properties);
            void sensorData(SensorData data);
            void frequencySpectrum(
                std::vector<double>& magnitude,
                int peakIndex);

        public:
            Controller();
            ~Controller();

            FFT_properties getFFTProperties();
            bool start();
            void stop();
            void getSensorSettings();
            void setSampleInterval(QString sampleInterval);

            // From FFT class
            std::vector<double>& getMagnitude();
            std::vector<double>& getRealPart();
            std::vector<double>& getImaginaryPart();
            fftw_complex *getIn();
            double indexToFrequency(int i);
    };

}

#endif // CONTROLLER_H
