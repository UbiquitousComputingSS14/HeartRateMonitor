#include "FFT.h"

#include <iostream>

#define DEFAULT_SAMPLES 256

namespace hrm
{

    FFT::FFT() : N(256)
    {
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_MEASURE);
    }

    FFT::~FFT()
    {
        fftw_destroy_plan(plan);
        fftw_free(in);
        fftw_free(out);
    }

    void FFT::computeFFT()//fftw_complex *in)
    {
        for (int i = 0; i < 256; ++i) {
            if (i <= 80) {
                in[i][0] = 0;
                in[i][1] = 0;
            } else if (i <= 160) {
                in[i][0] = 1;
                in[i][1] = 0;
            } else {
                in[i][0] = 0;
                in[i][1] = 0;
            }
        }

        fftw_execute(plan);
    }

    fftw_complex *FFT::getIn()
    {
        return in;
    }

    fftw_complex *FFT::getOut()
    {
        return out;
    }

}
