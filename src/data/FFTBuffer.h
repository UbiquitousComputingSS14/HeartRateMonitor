/**
 * This is a simple wrapper for the fftw_complex buffer.
 */

#ifndef FFT_BUFFER_H
#define FFT_BUFFER_H

#include <fftw3.h>

namespace hrm
{

    class FFTBuffer
    {
        private:
            fftw_complex *buffer;
            int size;

            int index;

        public:
            FFTBuffer(int size);
            ~FFTBuffer();

            void addData(int data);

            int getSize();
            fftw_complex *getBufferPointer();

    };

}

#endif
