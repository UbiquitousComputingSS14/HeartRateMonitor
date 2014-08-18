#include "FFT.h"

#include <cmath>
#include <iostream>
#include <limits>

#ifdef _WIN32
const static double M_PI = 3.14159265359;
#endif

namespace hrm
{

    FFT::FFT(double sampleInterval) : buffer(
            DEFAULT_SAMPLES,
            DEFAULT_ZERO_PADDING_SAMPLES,
            DEFAULT_SLIDING_WINDOW)
    {
        setSampleInterval(sampleInterval);
        applySampleSettings();

        properties.maxFrequency = DEFAULT_MAX_FREQUENCY;
        properties.minFrequency = DEFAULT_MIN_FREQUENCY;
    }

    FFT::~FFT()
    {
        fftw_destroy_plan(plan);
        fftw_free(out);
    }

    bool FFT::addSample(double sample)
    {
        // TODO: Because of the window function, the amplitude is not correct.
        // Therefore -> amplitude correction
        //in[index][0] = sin(2*M_PI*0.1*index*(0.07));

        if (buffer.add(sample) != nullptr) {
            // Got enough sample, do DFT.

            // Functions for input time domain.
            windowFunction();

            fftw_execute(plan);

            // Function for output frequency domain.
            idealFilter();
            scaleAndConvert();

            calculated = true; // For peak calculation
            return true;
        }

        return false;
    }

    void FFT::windowFunction()
    {
        for (int i = 0; i < properties.numberOfSamples; ++i) {
            // Hamming-window
            double windowValue = 0.54 - 0.46 * cos((2 * M_PI * i) / properties.numberOfSamples);
            buffer.update(i, buffer.getValue(i) * windowValue);
        }
    }

    void FFT::idealFilter()
    {
        for (int i = 1; i <= properties.outputSize; ++i) {
            if (!isRequiredFrequency(i)) {
                out[i][0] = 0.0;
                out[i][1] = 0.0;
            }
        }
    }

    void FFT::scaleAndConvert()
    {
        outReal.clear();
        outImaginary.clear();
        outMagnitude.clear();

        // Without DC offset
        for (int i = 1; i <= properties.outputSize; ++i) {
            // Complex value to magnitude
            double scaledAmplReal = 2.0 * out[i][0] / properties.totalSamples;
            double scaledAmplImag = 2.0 * out[i][1] / properties.totalSamples;
            double magnitude = (sqrt(pow(scaledAmplReal, 2) + pow(scaledAmplImag, 2)));

            outReal.push_back(scaledAmplReal);
            outImaginary.push_back(scaledAmplImag);
            outMagnitude.push_back(magnitude);
        }
    }

    void FFT::setSampleInterval(double sampleInterval)
    {
        properties.sampleInterval = sampleInterval;

        properties.sampleRate = 1.0 / (sampleInterval / 1000.0);
        properties.segmentDuration = properties.numberOfSamples * sampleInterval;
        properties.frequencyResolution = properties.sampleRate / properties.numberOfSamples;
        properties.frequencyResolutionWithZeroPadding = properties.sampleRate / properties.totalSamples;
    }

    void FFT::setSampleSettings(int effective, int zeroPad, int window)
    {
        buffer.setSize(effective, zeroPad, window);
        applySampleSettings();
        // Update the other properties
        setSampleInterval(properties.sampleInterval);
    }

    void FFT::applySampleSettings()
    {
        properties.numberOfSamples = buffer.getSize();
        properties.zeroPaddingSamples = buffer.getZeroPadSize();
        properties.slidingWindow = buffer.getWindowSize();
        properties.totalSamples = buffer.getTotalSize();
        // Without DC offset and only positive frequencies.
        properties.outputSize = properties.totalSamples / 2;

        if (out != nullptr)
            fftw_free(out);

        out = fftw_alloc_complex(properties.totalSamples);
        plan = fftw_plan_dft_1d(properties.totalSamples, buffer.get(), out,
                                FFTW_FORWARD, FFTW_MEASURE | FFTW_PRESERVE_INPUT);
    }

    int FFT::getPeak()
    {
        if (!calculated)
            return -1;

        double max = -1 * std::numeric_limits<double>::max();
        int indexMax = 0;

        for (int i = 1; i <= properties.outputSize; ++i) {
            if (!isRequiredFrequency(i))
                continue;

            if (outMagnitude[i-1] > max) {
                max = outMagnitude[i-1];
                indexMax = i - 1;
            }
        }

        return indexMax;
    }

    double FFT::indexToFrequency(int i)
    {
        return properties.sampleRate * (i / (double) properties.totalSamples);
    }

    bool FFT::isRequiredFrequency(int index)
    {
        double f = indexToFrequency(index);

        if (f < properties.minFrequency ||
                f > properties.maxFrequency)
            return false;
        return true;
    }

    fftw_complex *FFT::getIn()
    {
        return buffer.get();
    }

    fftw_complex *FFT::getOut()
    {
        if (calculated)
            return out;
        return nullptr;
    }

    std::vector<double>& FFT::getMagnitude()
    {
        return outMagnitude;
    }

    std::vector<double>& FFT::getRealPart()
    {
        return outReal;
    }

    std::vector<double>& FFT::getImaginaryPart()
    {
        return outImaginary;
    }

    FFT_properties FFT::getProperties()
    {
        return properties;
    }

}
