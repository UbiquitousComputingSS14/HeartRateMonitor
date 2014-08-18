#include "SettingsDialog.h"

#include <QString>

namespace hrm
{

    SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
    {
        setupUi(this);
    }

    SettingsDialog::~SettingsDialog()
    {
    }

    void SettingsDialog::setFFTInfo(FFT_properties properties)
    {
        fftSampleIntervalEdit->setText(QString::number(properties.sampleInterval));
        fftSampleRateEdit->setText(QString::number(properties.sampleRate));
        fftSamplesPerSegmentEdit->setText(QString::number(properties.numberOfSamples));
        fftZeroPaddingEdit->setText(QString::number(properties.zeroPaddingSamples));
        fftSegmentDurationEdit->setText(QString::number(properties.segmentDuration));

        QString str;

        str = QString::number(properties.frequencyResolution) + " (" +
              QString::number(properties.frequencyResolution * 60) + " bpm)";

        fftFrequencyResolutionEdit->setText(str);

        str = QString::number(properties.frequencyResolutionWithZeroPadding) + " (" +
              QString::number(properties.frequencyResolutionWithZeroPadding * 60) + " bpm)";

        fftFrequencyResolutionZPEdit->setText(str);
    }

    void SettingsDialog::setSensorInfo(SensorSettings settings)
    {
        sensorEdit->setText(settings.sensor);
        idEdit->setText(settings.id);
        maxValEdit->setText(settings.max);
        minValEdit->setText(settings.min);
        sampleIntervalEdit->setText(settings.sampleInterval);
        resolutionEdit->setText(settings.resolution);
        // TODO: setSampleIntervalEdit->setText(settings.sampleInterval);
    }

    void SettingsDialog::clearFrequencyDataEdit()
    {
        frequencyDataEdit->clear();
    }

    void SettingsDialog::clearTimeDataEdit()
    {
        timeDataEdit->clear();
    }

    void SettingsDialog::setFrequencyDataEdit(double magnitude)
    {
        frequencyDataEdit->append(QString::number(magnitude));
    }

    void SettingsDialog::setTimeDataEdit(double broadband)
    {
        timeDataEdit->append(QString::number(broadband));
    }

    void SettingsDialog::setPeakInfo(int indexMax, double fraction,
                                     double frequency, double max,
                                     double bpm)
    {
        peakIndexEdit->setText(QString::number(indexMax));
        peakFractionEdit->setText(QString::number(fraction));
        peakFrequencyEdit->setText(QString::number(frequency));
        peakAmplitudeEdit->setText(QString::number(max));
        peakBpmEdit->setText(QString::number(bpm));
    }

}
