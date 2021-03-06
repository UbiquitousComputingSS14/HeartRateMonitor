/**
 * This class is a wrapper for the fftw_complex array.
 *
 * It allows adding of data like a queue and returns a fftw_complex
 * buffer. If required, it can be configured to do zero padding.
 * Additionally, a sliding window is used. It determines the number of
 * new samples required to return a new fftw_complex buffer.
 *
 * @author Jens Gansloser
 */

#ifndef FFT_BUFFER_H
#define FFT_BUFFER_H

#include <vector>

#include <fftw3.h>

// Default sizes (in samples)
#define DEFAULT_SIZE 128
#define DEFAULT_WINDOW_SIZE 64
#define DEFAULT_ZERO_PAD_SIZE (4*DEFAULT_SIZE)

namespace hrm
{

    class FFTBuffer
    {
        private:
            int effectiveSize;
            int zeroPadSize;
            int totalSize;
            int windowSize;

            fftw_complex *dataOut = nullptr;
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

            /**
             * Normalizes the fftw_complex array with its mean.
             */
            void normalize();

            /**
             * @return Mean value of the vector buffer.
             */
            double getMean();

        public:
            /**
             * If (effectiveSize <= windowSize) => All data is
             * removed from the vector. Each fftw_complex array has "fresh" data.
             *
             * If (effectiveSize > windowSize && windowSize > 0) =>
             * (effectiveSize - windowSize) elements are preserved in
             * the data vector. windowSize new elements are needed
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

            /**
             * Update the value with index i in the fftw_complex array.
             */
            void update(int i, double value);

            /**
             * Get a value from the fftw_complex array.
             */
            double getValue(int i);

            /**
             * clears the internal data and sets a new size.
             */
            void setSize(int effective, int zeroPad, int window);

            fftw_complex *get();

            /**
             * @return effective size
             */
            unsigned int getSize();

            /**
             * @return effective + zeroPad size
             */
            unsigned int getTotalSize();

            /**
             * @return The sliding window size
             */
            int getWindowSize();

            int getZeroPadSize();
    };

}

#endif
