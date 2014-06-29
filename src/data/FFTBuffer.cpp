#include "FFTBuffer.h"

namespace hrm
{

    FFTBuffer::FFTBuffer(int size, int windowSize) : size(size)
    {
        dataOut = fftw_alloc_complex(size);
    }

    FFTBuffer::~FFTBuffer()
    {
        fftw_free(dataOut);
    }

    fftw_complex *FFTBuffer::add(double data)
    {
        this->data.push_back(data);

        if (this->data.size() == (unsigned int) size) {
            copyToDataOut();
            return dataOut;
        }

        return nullptr;
    }

    void FFTBuffer::copyToDataOut()
    {
        if (data.size() != (unsigned int) size)
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

}
