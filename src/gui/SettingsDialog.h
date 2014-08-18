#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "Serial.h"
#include "FFT.h"

#include "ui_SettingsDialog.h"

namespace hrm
{

    class SettingsDialog : public QDialog, public Ui_Dialog
    {
            Q_OBJECT

        public:
            SettingsDialog(QWidget *parent = 0);
            ~SettingsDialog();

            void setSensorInfo(SensorSettings settings);
            void setFFTInfo(FFT_properties properties);
            void setPeakInfo(int indexMax, double fraction,
                             double frequency, double max,
                             double bpm);

            void setTimeDataEdit(double broadband);
            void setFrequencyDataEdit(double magnitude);
            void clearTimeDataEdit();
            void clearFrequencyDataEdit();
    };

}

#endif
