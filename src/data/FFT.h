#ifndef FFT_H
#define FFT_H

#include <fftw3.h>

// power of 2
#define DEFAULT_SAMPLES 128

#define MIN_PULSE_FREQUENCY 0.5
#define MAX_PULSE_FREQUENCY 4.0

namespace hrm
{

    struct FFT_properties
    {
        double sampleInterval = 0.0; // delta x
        double sampleRate = 0.0; // Hz
        int numberOfSamples = 0; // N
        double segmentDuration = 0.0; // ms
    };

    class FFT
    {
        private:
            int index;

            FFT_properties properties;

            fftw_plan plan;

            fftw_complex *in;
            fftw_complex *out;
            double *outMagnitude;
            double *outReal;
            double *outImaginary;

            bool calculated = false;

            /**
             * Multiplicates the time domain input signal with a
             * window function (to weak the leakage effect).
             */
            void windowFunction();

            /**
             * Scales the frequency values to represent the correct
             * amplitude and converts the rectangular data to polar
             * coordinates.
             */
            void scaleAndConvert();

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

            void setSampleInterval(double sampleInterval);

            int getPeak();

            bool status();

            /**
             * @return The magnitude of the polar coordiantes without
             * the DC offset and only the positive frequency part.
             * This means it is (N/2) long.
             */
            double *getMagnitude();

            /**
             * @return the real (cos) scaled positive part of the frequencies.
             * This means it is (N/2) long.
             */
            double *getRealPart();

            /**
             * @return the imaginary (sin) scaled positive part of the frequencies.
             * This means it is (N/2) long.
             */
            double *getImaginaryPart();

            FFT_properties getProperties();
    };

}

#endif
