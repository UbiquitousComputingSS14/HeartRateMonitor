#include "FFTBuffer.h"

namespace hrm
{

    FFTBuffer::FFTBuffer(int size) : size(size), index(0)
    {
        buffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    }

    FFTBuffer::~FFTBuffer()
    {
        fftw_free(buffer);
    }

    void FFTBuffer::addData(int data)
    {
        buffer[index][0] = data;
        buffer[index][1] = 0;

        ++index;
    }

    int FFTBuffer::getSize()
    {
        return size;
    }

    fftw_complex *FFTBuffer::getBufferPointer()
    {
        return buffer;
    }

}
