#include "FFT.h"

#include <cmath>
#include <iostream>
#include <limits>

#ifdef _WIN32
const static double M_PI = 3.14159265359;
#endif

namespace hrm
{

    FFT::FFT() : buffer(DEFAULT_SAMPLES, DEFAULT_ZERO_PADDING_SAMPLES, DEFAULT_SLIDING_WINDOW)
    {
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
        //in[index][0] = sin(2*M_PI*0.1*index*(0.07)); // TODO: (test)

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
        for (unsigned int i = 0; i < buffer.getSize(); ++i) {
            // Hamming-window
            double windowValue = 0.54 - 0.46 * cos((2 * M_PI * i) / buffer.getSize());
            buffer.update(i, buffer.getValue(i) * windowValue);
        }
    }

    void FFT::idealFilter()
    {

    }

    void FFT::scaleAndConvert()
    {
        unsigned int N = buffer.getTotalSize();

        outReal.clear();
        outImaginary.clear();
        outMagnitude.clear();

        // Without DC offset
        for (unsigned int i = 1; i <= N / 2; ++i) {
            // Complex value to magnitude
            double scaledAmplReal = 2 * out[i][0] / N;
            double scaledAmplImag = 2 * out[i][1] / N;
            double magnitude = (sqrt(pow(scaledAmplReal, 2) + pow(scaledAmplImag, 2)));

            outReal.push_back(scaledAmplReal);
            outImaginary.push_back(scaledAmplImag);
            outMagnitude.push_back(magnitude);
        }
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

    void FFT::setSampleInterval(double sampleInterval)
    {
        properties.sampleInterval = sampleInterval;

        properties.sampleRate = 1.0 / (sampleInterval / 1000);
        properties.segmentDuration = properties.numberOfSamples * sampleInterval;
        properties.frequencyResolution = properties.sampleRate / properties.numberOfSamples;
        properties.frequencyResolutionWithZeroPadding = properties.sampleRate / properties.totalSamples;
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

        out = fftw_alloc_complex(buffer.getTotalSize());
        plan = fftw_plan_dft_1d(buffer.getTotalSize(), buffer.get(), out,
                                FFTW_FORWARD, FFTW_MEASURE | FFTW_PRESERVE_INPUT);
    }

    void FFT::setSampleSettings(int effective, int zeroPad, int window)
    {
        buffer.setSize(effective, zeroPad, window);
        applySampleSettings();
    }

    int FFT::getPeak()
    {
        if (!calculated)
            return -1;

        // Requires sample rate.
        if (!ready())
            return -1;

        unsigned int N = buffer.getTotalSize();

        double max = -1 * std::numeric_limits<double>::max();
        int indexMax = 0;

        for (unsigned int i = 1; i <= N / 2; ++i) {
            if (indexToFrequency(i) < properties.minFrequency ||
                    indexToFrequency(i) > properties.maxFrequency)
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
        if (!ready())
            return -1.0;

        return properties.sampleRate * (i / (double) properties.totalSamples);
    }

    bool FFT::ready()
    {
        if (properties.sampleInterval == 0.0 || properties.sampleRate == 0.0)
            return false;
        return true;
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
