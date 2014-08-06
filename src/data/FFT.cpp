#include "FFT.h"

#include <cmath>
#include <iostream>
#include <limits>

#ifdef _WIN32
    const static double M_PI = 3.14159265359;
#endif

namespace hrm
{

    FFT::FFT()
    {
        properties.numberOfSamples = DEFAULT_SAMPLES;
        properties.zeroPaddingSamples = DEFAULT_ZERO_PADDING_SAMPLES;
        properties.slidingWindow = SLIDING_WINDOW;
        properties.totalSamples = properties.numberOfSamples + properties.zeroPaddingSamples;

        // Filter determines the max and min frequencies.
        properties.maxFrequency = butterworth.getMaxFrequency();
        properties.minFrequency = butterworth.getMinFrequency();

        buffer = std::make_shared<FFTBuffer>(
                     properties.numberOfSamples,
                     properties.zeroPaddingSamples,
                     properties.slidingWindow);

        out = fftw_alloc_complex(buffer->getTotalSize());
        plan = fftw_plan_dft_1d(buffer->getTotalSize(), buffer->get(), out,
                                FFTW_FORWARD, FFTW_MEASURE | FFTW_PRESERVE_INPUT);

        // Without DC offset and only positive frequencies.
        properties.outputSize = buffer->getTotalSize() / 2;

        outMagnitude = new double[properties.outputSize];
        outReal = new double[properties.outputSize];
        outImaginary = new double[properties.outputSize];
    }

    FFT::~FFT()
    {
        fftw_destroy_plan(plan);
        fftw_free(out);

        delete[] outMagnitude;
        delete[] outReal;
        delete[] outImaginary;
    }

    bool FFT::addSample(double sample)
    {
        // Because of the window function, the amplitude is not correct.
        // Therefore -> amplitude correction

        //in[index][0] = sin(2*M_PI*0.1*index*(0.07)); // TODO: (test)

        if (buffer->add(sample) != nullptr) {
            // Got enough sample, do DFT.

            // Functions for input time domain.
            butterworth.filter(buffer.get(), 25);
            windowFunction();

            fftw_execute(plan);

            // Function for output frequency domain.
            scaleAndConvert();

            calculated = true; // For peak calculation
            return true;
        }

        return false;
    }

    void FFT::windowFunction()
    {
        for (unsigned int i = 0; i < buffer->getSize(); ++i) {
            // Hamming-window
            double windowValue = 0.54 - 0.46 * cos((2 * M_PI * i) / buffer->getSize());
            buffer->update(i, buffer->getValue(i) * windowValue);
        }
    }

    void FFT::scaleAndConvert()
    {
        unsigned int N = buffer->getTotalSize();

        // Without DC offset
        for (unsigned int i = 1; i <= N / 2; ++i) {
            // Complex value to magnitude
            double scaledAmplReal = 2 * out[i][0] / N;
            double scaledAmplImag = 2 * out[i][1] / N;
            double magnitude = (sqrt(pow(scaledAmplReal, 2) + pow(scaledAmplImag, 2)));

            outReal[i-1] = scaledAmplReal;
            outImaginary[i-1] = scaledAmplImag;
            outMagnitude[i-1] = magnitude;
        }
    }

    fftw_complex *FFT::getIn()
    {
        return buffer->get();
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

    int FFT::getPeak()
    {
        if (!calculated)
            return -1;

        if (!ready())
            return -1;

        unsigned int N = buffer->getTotalSize();

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

    bool FFT::ready()
    {
        if (properties.sampleInterval == 0.0 || properties.sampleRate == 0.0)
            return false;
        return true;
    }

    double *FFT::getMagnitude()
    {
        return outMagnitude;
    }

    double *FFT::getRealPart()
    {
        return outReal;
    }

    double *FFT::getImaginaryPart()
    {
        return outImaginary;
    }

    double FFT::indexToFrequency(int i)
    {
        if (!ready())
            return -1.0;

        return properties.sampleRate * (i / (double) properties.totalSamples);
    }

    FFT_properties FFT::getProperties()
    {
        return properties;
    }

}
