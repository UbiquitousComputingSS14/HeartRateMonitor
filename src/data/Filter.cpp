#include "Filter.h"

namespace hrm
{

    Filter::Filter() :
        nzeros(NZEROS), npoles(NPOLES), gain(GAIN),
        minFrequency(MIN_PULSE_FREQUENCY), maxFrequency(MAX_PULSE_FREQUENCY),
        sampleRate(SAMPLE_RATE)
    {
    }

    void Filter::filter(FFTBuffer *buffer, int cut)
    {
        for (int i = 0; i < nzeros + 1; ++i)
            xv.push_back(0.0);

        for (int i = 0; i < npoles + 1; ++i)
            yv.push_back(0.0);

        for (unsigned int i = 0; i < buffer->getSize(); ++i) {
            xv[0] = xv[1];
            xv[1] = xv[2];
            xv[2] = xv[3];
            xv[3] = xv[4];
            xv[4] = buffer->getValue(i) / gain;
            yv[0] = yv[1];
            yv[1] = yv[2];
            yv[2] = yv[3];
            yv[3] = yv[4];
            yv[4] =   (xv[0] + xv[4]) - 2 * xv[2]
                      + ( -0.4128015981 * yv[0]) + (  0.2397611203 * yv[1])
                      + (  0.9889943457 * yv[2]) + ( -0.6474311512 * yv[3]);
            buffer->update(i, yv[4]);
        }

        // TODO: This is bad!! make the buffer smaller or zero pad

        // Cut stabilization time
        for (unsigned int i = 0; i < buffer->getSize() - cut; ++i)
            buffer->update(i, buffer->getValue(i + cut));

        for (unsigned int i = buffer->getSize() - cut; i < buffer->getSize(); ++i)
            buffer->update(i, 0);
    }

    double Filter::getMinFrequency()
    {
        return minFrequency;
    }

    double Filter::getMaxFrequency()
    {
        return maxFrequency;
    }

}
