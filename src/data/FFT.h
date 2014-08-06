#ifndef FFT_H
#define FFT_H

#include <memory>

#include "FFTBuffer.h"
#include "Filter.h"

#include <fftw3.h>

// power of 2
#define TOTAL_SAMPLES 1024
#define DEFAULT_SAMPLES 128
#define SLIDING_WINDOW 5

#define DEFAULT_ZERO_PADDING_SAMPLES (TOTAL_SAMPLES - DEFAULT_SAMPLES)

namespace hrm
{

    struct FFT_properties {
        int numberOfSamples = 0;
        int zeroPaddingSamples = 0;
        int totalSamples = 0; // N (numberOfSamples + zeroPaddingSamples)
        int outputSize = 0; // totalSamples / 2
        int slidingWindow = 0;

        double sampleInterval = 0.0; // delta x
        double sampleRate = 0.0; // Hz
        double minFrequency = 0.0;
        double maxFrequency = 0.0;

        double segmentDuration = 0.0; // ms
        double frequencyResolution = 0.0; // Hz
        double frequencyResolutionWithZeroPadding = 0.0; // Hz
    };

    class FFT
    {
        private:
            FFT_properties properties;
            Filter butterworth;

            fftw_plan plan;
            std::shared_ptr<FFTBuffer> buffer;
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

            /**
             * FFT needs the sample interval to start calculating.
             * Call setSampleInterval() first.
             */
            bool ready();

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
