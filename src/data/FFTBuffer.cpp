#include "FFTBuffer.h"

#include <iostream>

namespace hrm
{

    FFTBuffer::FFTBuffer(int effective, int zeroPad, int window) :
        effectiveSize(effective),
        zeroPadSize(zeroPad),
        windowSize(window)
    {
        dataOut = fftw_alloc_complex(effectiveSize + zeroPadSize);
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
        for (int i = effectiveSize; i < (effectiveSize + zeroPadSize); ++i) {
            dataOut[i][0] = 0.0;
            dataOut[i][1] = 0.0;
        }
    }

    void FFTBuffer::update(int i, double value)
    {
        if (i >= effectiveSize + zeroPadSize || i <= -1)
            return;

        dataOut[i][0] = value;
    }

    double FFTBuffer::getValue(int i)
    {
        if (i >= effectiveSize + zeroPadSize || i <= -1)
            return 0.0;

        return dataOut[i][0];
    }

    fftw_complex *FFTBuffer::get()
    {
        return dataOut;
    }

    unsigned int FFTBuffer::size()
    {
        return effectiveSize;
    }

    unsigned int FFTBuffer::totalSize()
    {
        return effectiveSize + zeroPadSize;
    }

}
