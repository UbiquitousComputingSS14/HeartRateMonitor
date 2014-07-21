#include "FFT.h"

#include <iostream>
#include <cmath>
#include <limits>

// Filter properties
#define NZEROS 4
#define NPOLES 4
#define GAIN 1.564354775e+00
static float xv[NZEROS+1], yv[NPOLES+1];

namespace hrm
{

    FFT::FFT()
    {
        properties.numberOfSamples = DEFAULT_SAMPLES;
        properties.zeroPaddingSamples = DEFAULT_ZERO_PADDING_SAMPLES;
        properties.slidingWindow = SLIDING_WINDOW;
        properties.totalSamples = properties.numberOfSamples + properties.zeroPaddingSamples;

        buffer = std::make_shared<FFTBuffer>(
                     properties.numberOfSamples,
                     properties.zeroPaddingSamples,
                     properties.slidingWindow);

        out = fftw_alloc_complex(buffer->totalSize());
        plan = fftw_plan_dft_1d(buffer->totalSize(), buffer->get(), out,
                                FFTW_FORWARD, FFTW_MEASURE | FFTW_PRESERVE_INPUT);

        // Without DC offset and only positive frequencies.
        properties.outputSize = buffer->totalSize() / 2;

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
            filter();
            windowFunction();

            fftw_execute(plan);

            // Function for output frequency domain.
            scaleAndConvert();

            calculated = true; // For peak calculation
            return true;
        }

        return false;
    }

    void FFT::filter()
    {
        for (int i = 0; i < 5; ++i) {
            xv[i] = 0;
            yv[i] = 0;
        }

        for (unsigned int i = 0; i < buffer->size(); ++i) {
            xv[0] = xv[1];
            xv[1] = xv[2];
            xv[2] = xv[3];
            xv[3] = xv[4];
            xv[4] = buffer->getValue(i) / GAIN;
            yv[0] = yv[1];
            yv[1] = yv[2];
            yv[2] = yv[3];
            yv[3] = yv[4];
            yv[4] =   (xv[0] + xv[4]) - 2 * xv[2]
                      + ( -0.4128015981 * yv[0]) + (  0.2397611203 * yv[1])
                      + (  0.9889943457 * yv[2]) + ( -0.6474311512 * yv[3]);
            buffer->update(i, yv[4]);
        }

        // Cut stabilization time (25 samples)
        for (unsigned int i = 0; i < buffer->size() - 25; ++i) {
            buffer->update(i, buffer->getValue(i + 25));
        }
    }

    void FFT::windowFunction()
    {
        for (unsigned int i = 0; i < buffer->size(); ++i) {
            // Hamming-window
            double windowValue = 0.54 - 0.46 * cos((2 * M_PI * i) / buffer->size());

            buffer->update(i, buffer->getValue(i) * windowValue);
        }
    }

    void FFT::scaleAndConvert()
    {
        unsigned int N = buffer->totalSize();

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
        unsigned int N = buffer->totalSize();

        if (!calculated)
            return -1;

        double max = -1 * std::numeric_limits<double>::max();
        int indexMax = 0;

        for (unsigned int i = 1; i <= N / 2; ++i) {
            if (indexToFrequency(i) < MIN_PULSE_FREQUENCY ||
                    indexToFrequency(i) > MAX_PULSE_FREQUENCY)
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
        return properties.sampleRate * (i / (double) properties.totalSamples);
    }

    FFT_properties FFT::getProperties()
    {
        return properties;
    }

}
