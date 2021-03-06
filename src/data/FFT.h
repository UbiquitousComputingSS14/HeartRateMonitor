/**
 * The class FFT executes the signal processing steps to determine the
 * heart rate.
 *
 * @author Jens Gansloser
 */

#ifndef FFT_H
#define FFT_H

#include <memory>

#include "FFTBuffer.h"

#include <fftw3.h>

// Default values
#define DEFAULT_TOTAL_SAMPLES 1024 // power of 2
#define DEFAULT_SAMPLES 128
#define DEFAULT_SLIDING_WINDOW 5

#define DEFAULT_ZERO_PADDING_SAMPLES (DEFAULT_TOTAL_SAMPLES - DEFAULT_SAMPLES)

#define DEFAULT_MIN_FREQUENCY 0.7 // Hz
#define DEFAULT_MAX_FREQUENCY 3.9 // Hz

namespace hrm
{

    struct FFT_properties {
        int numberOfSamples = 0;
        int zeroPaddingSamples = 0;
        int totalSamples = 0; // N (numberOfSamples + zeroPaddingSamples)
        int outputSize = 0; // totalSamples / 2 (only positive frequencies)

        int slidingWindow = 0;

        // Set from outside.
        double sampleInterval = 0.0; // delta x
        // Determines:
        double sampleRate = 0.0; // Hz
        double segmentDuration = 0.0; // ms
        double frequencyResolution = 0.0; // Hz
        double frequencyResolutionWithZeroPadding = 0.0; // Hz

        // What to use for ideal bandpass filter.
        double minFrequency = 0.0; // Hz
        double maxFrequency = 0.0; // Hz
    };

    class FFT
    {
        private:
            FFT_properties properties;

            fftw_plan plan;
            fftw_complex *out = nullptr;
            FFTBuffer buffer;

            std::vector<double> outMagnitude;
            std::vector<double> outReal;
            std::vector<double> outImaginary;

            bool useWindowFunction = true;
            bool useIdealFilter = true;
            bool useScaling = true;
            bool calculated = false;

            /**
             * Multiplicates the time domain input signal with a
             * window function (to weak the leakage effect).
             */
            void windowFunction_Hamming();
            void windowFunction_Hanning();

            /**
             * Ideal filter to remove unwanted frequencies.
             */
            void idealFilter();

            /**
             * Scales the frequency values to represent the correct
             * amplitude and converts the rectangular data to polar
             * coordinates. (+ and - frequency because of the complex
             * fft, [comparison to real fft]).
             */
            void scaleAndConvert();

            /**
             * Only converts the rectangular data to polar
             * coordinates.
             */
            void convert();

            /**
             * Applies the buffer settings to the fft output settings.
             * (Number of samples used for FFT)
             */
            void applySampleSettings();

        public:
            FFT(double sampleInterval);
            ~FFT();

            /**
             * Ad a sample to the internal buffer. When N (max sample size)
             * is reached, calculate the DFT.
             *
             * @retval true Got enough data and calculated the DFT.
             * @retval false Not enough data to calculate the DFT.
             */
            bool addSample(double sample);

            /**
             * Is required to calculate the peak.
             */
            void setSampleInterval(double sampleInterval);

            void setSampleSettings(int effective, int zeroPad, int window);

            void setUseFilter(bool status);

            void setUseWindowFunction(bool status);

            void setUseScaling(bool status);

            int getPeak();

            double indexToFrequency(int i);

            /**
            * Checks if the given index from a frequency spectrum is
            * a required frequency based on the min and max frequency
            * variable.
            *
            * @retval true The frequency is required
            * @retval false The frequency is not required (can be removed)
            */
            bool isRequiredFrequency(int index);

            /**
             * @return Input array held by FFTBuffer.
             */
            fftw_complex *getIn();

            fftw_complex *getOut();

            /**
             * @return The magnitude of the polar coordiantes without
             * the DC offset and only the positive frequency part.
             * This means it is (N/2) long.
             */
            std::vector<double>& getMagnitude();

            /**
             * @return the real (cos) scaled positive part of the frequencies.
             * This means it is (N/2) long.
             */
            std::vector<double>& getRealPart();

            /**
             * @return the imaginary (sin) scaled positive part of the frequencies.
             * This means it is (N/2) long.
             */
            std::vector<double>& getImaginaryPart();

            FFT_properties getProperties();
    };

}

#endif
