#include "FFT.h"

#include <iostream>

#include <cmath>

#define DEFAULT_SAMPLES 64

namespace hrm
{

    FFT::FFT() : N(DEFAULT_SAMPLES), index(0)
    {
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_MEASURE);

        N = DEFAULT_SAMPLES;
        properties.numberOfSamples = DEFAULT_SAMPLES;
    }

    FFT::~FFT()
    {
        fftw_destroy_plan(plan);
        fftw_free(in);
        fftw_free(out);
    }

    bool FFT::addSample(double sample)
    {
        in[index][0] = sample;
        in[index][1] = 0;

        ++index;

        if (index == N) {
            // Got enough sample, do DFT.

            windowFunction();
            fftw_execute(plan);
            index = 0;

            return true;
        }

        return false;
    }

    void FFT::windowFunction()
    {
        for (int i = 0; i < N; ++i) {
            // Hamming-window
            double windowValue = 0.54 - 0.46 * cos((2*M_PI*i)/N);

            in[i][0] = in[i][0] * windowValue;
            in[i][1] = in[i][1] * windowValue;
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

    FFT_properties FFT::getProperties()
    {
        return properties;
    }

    int FFT::getN() {
        return N;
    }

}
