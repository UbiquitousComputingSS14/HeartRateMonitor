#ifndef FFT_H
#define FFT_H

#include "FFTBuffer.h"

#include <fftw3.h>

// power of 2
#define DEFAULT_SAMPLES 100
#define DEFAULT_ZERO_PADDING_SAMPLES (4*DEFAULT_SAMPLES)

#define MIN_PULSE_FREQUENCY 0.7
#define MAX_PULSE_FREQUENCY 3.9

namespace hrm
{

    struct FFT_properties {
        int numberOfSamples = 0;
        int zeroPaddingSamples = 0;
        int totalSamples = 0; // N

        double sampleInterval = 0.0; // delta x
        double sampleRate = 0.0; // Hz

        double segmentDuration = 0.0; // ms
        double frequencyResolution = 0.0; // Hz
        double frequencyResolutionWithZeroPadding = 0.0; // Hz
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
             * coordinates. (+ and - frequency because of the complex
             * fft, [comparison to real fft]).
             */
            void scaleAndConvert();

            /**
             * Does NOT give more information.
             */
            void zeroPad();

            void filter();

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

            double indexToFrequency(int i);

            FFT_properties getProperties();
    };

}

#endif
