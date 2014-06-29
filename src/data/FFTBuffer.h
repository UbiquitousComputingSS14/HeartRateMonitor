#ifndef FFT_BUFFER_H
#define FFT_BUFFER_H

#include <vector>

#include <fftw3.h>

#define DEFAULT_SIZE 128
#define DEFAULT_WINDOW_SIZE 1

namespace hrm
{

    class FFTBuffer
    {
        private:
            int size;
            int windowSize;

            fftw_complex *dataOut;

            std::vector<double> data;

            /**
             * Copy data from data vector to dataOut and preserve window
             * elements in the data vector.
             */
            void copyToDataOut();

        public:
            /**
             * If (windowSize >= size || windowSize <= 0) => All data is
             * removed from the vector. Each fftw_complex array has "fresh" data.
             *
             * If (windowSize < size) => size - windowSize elements are
             * preserved in the data vector. windowSize new elements are needed
             * that add() returns the next array pointer.
             */
            FFTBuffer(int size = DEFAULT_SIZE, int windowSize = DEFAULT_WINDOW_SIZE);
            ~FFTBuffer();

            /**
             * Adds the data as real value to the buffer.
             *
             * @retval nullptr Added value, but buffer is not full.
             * @retval address Address of the fftw_complex buffer. Buffer is full.
             */
            fftw_complex *add(double data);
    };

}

#endif
