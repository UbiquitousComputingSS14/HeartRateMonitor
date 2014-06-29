/**
 * This class is a wrapper for the fftw_complex array.
 */

#ifndef FFT_BUFFER_H
#define FFT_BUFFER_H

#include <vector>

#include <fftw3.h>

#define DEFAULT_SIZE 128 // Number of samples
#define DEFAULT_WINDOW_SIZE 64
#define DEFAULT_ZERO_PAD_SIZE (4*DEFAULT_SIZE)

namespace hrm
{

    class FFTBuffer
    {
        private:
            int effectiveSize;
            int zeroPadSize;
            int windowSize;

            fftw_complex *dataOut;
            std::vector<double> data;

            /**
             * Copy data from data vector to dataOut and preserve windowSize
             * elements in the data vector.
             */
            void copyToDataOut();

            /**
             * Zero pad the last zeroPadSize elements in dataOut.
             */
            void zeroPad();

        public:
            /**
             * If (windowSize >= effectiveSize || windowSize <= 0) => All data is
             * removed from the vector. Each fftw_complex array has "fresh" data.
             *
             * If (windowSize < effectiveSize) => effectiveSize - windowSize elements are
             * preserved in the data vector. windowSize new elements are needed
             * that add() returns the next array pointer.
             */
            FFTBuffer(int effective = DEFAULT_SIZE,
                      int zeroPad = DEFAULT_ZERO_PAD_SIZE,
                      int window = DEFAULT_WINDOW_SIZE);
            ~FFTBuffer();

            /**
             * Adds the data as real value to the buffer. If this returns
             * no nullptr, dataOut consists of valid data. The return value
             * is zero padded.
             *
             * @retval nullptr Added value, but buffer is not full.
             * @retval address Address of the fftw_complex buffer. Buffer is full.
             */
            fftw_complex *add(double p_data);

            void update(int i, double value);

            double getValue(int i);

            fftw_complex *get();

            unsigned int size();

            unsigned int totalSize();
    };

}

#endif
