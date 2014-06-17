#ifndef FFT_H
#define FFT_H

#include <fftw3.h>

namespace hrm
{

    class FFT
    {
        private:
            int N;

            fftw_plan plan;
            fftw_complex *in;
            fftw_complex *out;

        public:
            FFT();
            ~FFT();

            void computeFFT();//fftw_complex *in);
            fftw_complex *getIn();
            fftw_complex *getOut();
    };

}

#endif
