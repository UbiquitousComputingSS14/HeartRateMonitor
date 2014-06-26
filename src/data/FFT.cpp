#include "FFT.h"

#include <iostream>
#include <cmath>
#include <limits>

namespace hrm
{

    FFT::FFT() : index(0)
    {
        properties.numberOfSamples = DEFAULT_SAMPLES;
        properties.zeroPaddingSamples = DEFAULT_ZERO_PADDING_SAMPLES;

        int inputSize = properties.numberOfSamples;// TODO: + properties.zeroPaddingSamples;

        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * inputSize);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * inputSize);
        plan = fftw_plan_dft_1d(inputSize, in, out, FFTW_FORWARD, FFTW_MEASURE);

        //in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * properties.numberOfSamples);
        //out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * properties.numberOfSamples);
        //plan = fftw_plan_dft_1d(properties.numberOfSamples, in, out, FFTW_FORWARD, FFTW_MEASURE);

        // Without DC offset
        int outputSize = properties.numberOfSamples / 2;

        outMagnitude = new double[outputSize];
        outReal = new double[outputSize];
        outImaginary = new double[outputSize];
    }

    FFT::~FFT()
    {
        fftw_destroy_plan(plan);
        fftw_free(in);
        fftw_free(out);

        delete[] outMagnitude;
        delete[] outReal;
        delete[] outImaginary;
    }

    bool FFT::addSample(double sample)
    {
        // Because of the window function, the amplitude is not correct.
        // Therefore -> amplitude correction
        //in[index][0] = sin(2*M_PI*1.6*index*(0.07)); // TODO: (test)
        in[index][0] = sample;
        in[index][1] = 0;

        ++index;

        if (index == properties.numberOfSamples) {
            // Got enough sample, do DFT.

            windowFunction();
            zeroPad();
            fftw_execute(plan);
            scaleAndConvert();
            index = 0;
            calculated = true;

            return true;
        }

        return false;
    }

    void FFT::windowFunction()
    {
        for (int i = 0; i <= properties.numberOfSamples - 1; ++i) {
            // Hamming-window
            double windowValue = 0.54 - 0.46 * cos((2 * M_PI * i) / properties.numberOfSamples);

            in[i][0] = in[i][0] * windowValue;
            in[i][1] = in[i][1] * windowValue;
        }
    }

    void FFT::scaleAndConvert()
    {
        int N = properties.numberOfSamples;

        // Look only in the neede area.
        for (int i = 1; i <= N / 2; ++i) {
            // Complex value to magnitude
            double scaledAmplReal = 2 * out[i][0] / N;
            double scaledAmplImag = 2 * out[i][1] / N;
            double magnitude = (sqrt(pow(scaledAmplReal, 2) + pow(scaledAmplImag, 2)));

            outReal[i-1] = scaledAmplReal;
            outImaginary[i-1] = scaledAmplImag;
            outMagnitude[i-1] = magnitude;
        }
    }

    void FFT::zeroPad()
    {
        for (int i = 0; i < properties.zeroPaddingSamples; ++i) {
            in[index][0] = 0;
            in[index][1] = 0;

            ++index;
        }
    }

    fftw_complex *FFT::getIn()
    {
        return in;
    }

    fftw_complex *FFT::getOut()
    {
        return out;
    }

    void FFT::setSampleInterval(double sampleInterval)
    {
        properties.sampleInterval = sampleInterval;
        properties.sampleRate = 1.0 / (sampleInterval / 1000);
        properties.segmentDuration = properties.numberOfSamples * sampleInterval;
        properties.frequencyResolution = properties.sampleRate / properties.numberOfSamples;
    }

    int FFT::getPeak()
    {
        int N = properties.numberOfSamples;
        int sampleRate = properties.sampleRate;

        if (!calculated)
            return -1;

        double max = -1 * std::numeric_limits<double>::max();
        int indexMax = 0;

        for (int i = 1; i <= N / 2; ++i) {
            if (sampleRate * (i / (double) N) < MIN_PULSE_FREQUENCY ||
                    sampleRate * (i / (double) N) > MAX_PULSE_FREQUENCY)
                continue;

            if (outMagnitude[i-1] > max) {
                max = outMagnitude[i-1];
                indexMax = i-1;
            }
        }

        return indexMax;
    }

    bool FFT::status()
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

    FFT_properties FFT::getProperties()
    {
        return properties;
    }

}
