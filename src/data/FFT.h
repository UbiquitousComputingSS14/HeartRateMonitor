#ifndef FFT_H
#define FFT_H

#include <fftw3.h>

namespace hrm
{

    struct FFT_properties
    {
        int sampleInterval;
        int sampleRate;
        int numberOfSamples;
    };

    class FFT
    {
        private:
            int N; // Number of samples
            int index;

            FFT_properties properties;

            fftw_plan plan;

            fftw_complex *in;
            fftw_complex *out;

            /**
             * Multiplicates the time domain input signal with a
             * window function (to weak the leakage effect).
             */
            void windowFunction();

        public:
            FFT();
            ~FFT();

            /**
             * Ad a sample to the internal buffer. When N (max sample size)
             * is reached, calculate the DFT.
             *
             * @retval true Got enough data and calculated the DFT.
             * @retval false Not enough data to calculate the DFT.
             */
            bool addSample(double sample);

            fftw_complex *getIn();
            fftw_complex *getOut();
            int getN();
            FFT_properties getProperties();
    };

}

#endif
