#include "Controller.h"

namespace hrm
{

    const QString Controller::GET_SETTINGS_SERIAL = "get settings";
    const QString Controller::SET_SAMPLE_INTERVAL_SERIAL = "set sampleInterval ";

    Controller::Controller()
    {
        serial = new Serial();

        initSignals();
    }

    void Controller::initSignals()
    {
        connect(serial, SIGNAL(receiveSensorData(SensorData)),
                this, SLOT(receiveSensorData(SensorData)));
        connect(serial, SIGNAL(receiveSensorSettings(SensorSettings)),
                this, SLOT(receiveSensorSettings(SensorSettings)));
    }

    Controller::~Controller()
    {
        serial->closeSerial();
        delete serial;
    }

    void Controller::receiveSensorData(SensorData data)
    {
        if (!fft) {
            getSensorSettings();
            return;
        }

        if (fft->addSample(data.broadband))
            Q_EMIT frequencySpectrum(fft->getMagnitude(),
                                     fft->getPeak());

        Q_EMIT sensorData(data);
    }

    void Controller::receiveSensorSettings(SensorSettings settings)
    {
        if (!fft)
            fft = std::unique_ptr<FFT>(new FFT(settings.sampleInterval.toDouble()));
        else
            fft->setSampleInterval(settings.sampleInterval.toDouble());

        FFT_properties properties = fft->getProperties();

        Q_EMIT sensorSettings(settings, properties);
    }

    FFT_properties Controller::getFFTProperties()
    {
        return fft->getProperties();
    }

    bool Controller::start()
    {
        if (serial->openSerial()) {
            getSensorSettings();
            return true;
        }

        return false;
    }

    void Controller::stop()
    {
        serial->closeSerial();
    }

    void Controller::getSensorSettings()
    {
        QString data = GET_SETTINGS_SERIAL;
        serial->sendData(data + '\n');
    }

    void Controller::setSampleInterval(QString sampleInterval)
    {
        QString data = SET_SAMPLE_INTERVAL_SERIAL + sampleInterval;
        serial->sendData(data + "\n");

        getSensorSettings();
    }

    std::vector<double>& Controller::getMagnitude()
    {
        return fft->getMagnitude();
    }

    std::vector<double>& Controller::getRealPart()
    {
        return fft->getRealPart();
    }

    std::vector<double>& Controller::getImaginaryPart()
    {
        return fft->getImaginaryPart();
    }

    fftw_complex *Controller::getIn()
    {
        return fft->getIn();
    }

    double Controller::indexToFrequency(int i)
    {
        return fft->indexToFrequency(i);
    }

    void Controller::setEffectiveSize(int size)
    {
        fft->setSampleSettings(size,
                               fft->getProperties().zeroPaddingSamples,
                               fft->getProperties().slidingWindow);
    }

    void Controller::setSlidingWindowSize(int size)
    {
        fft->setSampleSettings(fft->getProperties().numberOfSamples,
                               size,
                               fft->getProperties().slidingWindow);
    }

    void Controller::setZeroPadSize(int size)
    {
        fft->setSampleSettings(fft->getProperties().numberOfSamples,
                               fft->getProperties().zeroPaddingSamples,
                               size);
    }

}
