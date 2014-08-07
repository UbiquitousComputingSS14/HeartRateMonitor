#include "FFTBuffer.h"

namespace hrm
{

    FFTBuffer::FFTBuffer(int effective, int zeroPad, int window)
    {
        setSize(effective, zeroPad, window);
    }

    FFTBuffer::~FFTBuffer()
    {
        fftw_free(dataOut);
    }

    fftw_complex *FFTBuffer::add(double p_data)
    {
        data.push_back(p_data);

        if (data.size() == (unsigned int) effectiveSize) {
            copyToDataOut();

            // Zero pad the other values.
            zeroPad();

            return dataOut;
        }

        return nullptr;
    }

    void FFTBuffer::copyToDataOut()
    {
        if (data.size() != (unsigned int) effectiveSize)
            return;

        int i = 0;

        for (auto it = data.begin(); it != data.end();) {
            if (i < windowSize) {
                dataOut[i][0] = *it;
                data.erase(it);
            } else {
                dataOut[i][0] = *it;
                ++it;
            }
            dataOut[i][1] = 0;

            ++i;
        }
    }

    void FFTBuffer::zeroPad()
    {
        for (int i = effectiveSize; i < totalSize; ++i) {
            dataOut[i][0] = 0.0;
            dataOut[i][1] = 0.0;
        }
    }

    void FFTBuffer::update(int i, double value)
    {
        if (i >= totalSize || i <= -1)
            return;

        dataOut[i][0] = value;
    }

    double FFTBuffer::getValue(int i)
    {
        if (i >= totalSize || i <= -1)
            return 0.0;

        return dataOut[i][0];
    }

    void FFTBuffer::setSize(int effective, int zeroPad, int window)
    {
        if (dataOut != nullptr)
            fftw_free(dataOut);

        dataOut = fftw_alloc_complex(effectiveSize + zeroPadSize);

        data.clear();

        effectiveSize = effective;
        zeroPadSize = zeroPad;
        totalSize = effectiveSize + zeroPadSize;
        windowSize = window;
    }

    fftw_complex *FFTBuffer::get()
    {
        return dataOut;
    }

    unsigned int FFTBuffer::getSize()
    {
        return effectiveSize;
    }

    unsigned int FFTBuffer::getTotalSize()
    {
        return totalSize;
    }

    int FFTBuffer::getWindowSize()
    {
        return windowSize;
    }

    int FFTBuffer::getZeroPadSize()
    {
        return zeroPadSize;
    }

}
